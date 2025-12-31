/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tree.h"
#include "blob.h"
#include "gitglobal_p.h"
#include "oid.h"
#include "qdebug.h"
#include "types.h"

#include <git2/commit.h>
#include <git2/revparse.h>
#include <git2/tree.h>

#include <QDir>
#include <QFileInfo>

namespace Git
{

class TreePrivate
{
public:
    explicit TreePrivate(Tree *parent, git_tree *tree = nullptr);
    ~TreePrivate();

    Tree *q;
    QList<TreeEntry *> entries;
    QMultiMap<QString, TreeEntry> treeData;

    git_tree *tree{nullptr};

    void initTree();
    void browseNestedEntities(EntryType type, const QString &path, QStringList &list) const;
};

Tree::Tree()
    : d{new TreePrivate{this, nullptr}}
{
}

Tree::Tree(git_tree *tree)
    : d{new TreePrivate{this, tree}}
{
    if (tree)
        d->initTree();
}

Tree::Tree(git_repository *repo, const QString &place)
    : d{new TreePrivate{this}}
{
    git_tree *tree;
    git_object *placeObject{nullptr};
    git_commit *commit{nullptr};

    SequenceRunner r;
    r.run(git_revparse_single, &placeObject, repo, place.toLatin1().constData());
    r.run(git_commit_lookup, &commit, repo, git_object_id(placeObject));
    r.run(git_commit_tree, &tree, commit);

    if (r.isSuccess()) {
        d->tree = tree;
        d->initTree();
    }
}

Tree::Tree(const Tree &other)
    : d{other.d}
{
}

Tree &Tree::operator=(const Tree &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Tree::operator==(const Tree &other) const
{
    return d->tree == other.d->tree;
}

bool Tree::operator!=(const Tree &other) const
{
    return !(*this == other);
}

bool Tree::isNull() const
{
    return !d->tree;
}

git_tree *Tree::data() const
{
    return d->tree;
}

const git_tree *Tree::constData() const
{
    return d->tree;
}

TreeEntryLists Tree::entries(const QString &path) const
{
    return TreeEntryLists{d->treeData.values(path)};
}

QStringList Tree::entries(const QString &path, EntryType filter) const
{
    auto entries = this->entries(path);
    QStringList ret;
    for (const auto &en : std::as_const(entries))
        if (filter == EntryType::All || en.type == filter)
            ret << en.name;
    return ret;
}

QStringList Tree::entries(EntryType filter) const
{
    QStringList list;

    d->browseNestedEntities(filter, QString(), list);

    return list;
}

Blob Tree::file(const QString &path) const
{
    git_tree_entry *entry;
    BEGIN

    auto p = path;
    if (p.startsWith("/"))
        p = p.remove(0, 1);

    auto filePath = p.mid(0, p.lastIndexOf("/"));

    STEP git_tree_entry_bypath(&entry, d->tree, toConstChars(p));

    if (IS_ERROR)
        return Blob{};

    return Blob{git_tree_owner(d->tree), entry, filePath};
}

bool Tree::extract(const QString &destinationFolder, const QString &prefix)
{
    struct wrapper {
        QString destinationFolder;
        QString prefix;
        Tree *tree = nullptr;
    };

    auto cb = [](const char *root, const git_tree_entry *entry, void *payload) -> int {
        auto type = git_tree_entry_type(entry);

        if (type != GIT_OBJECT_BLOB)
            return 0;

        auto w = reinterpret_cast<wrapper *>(payload);
        QString path{root};
        // if (path.endsWith("/"))
        // path = path.mid(0, path.lastIndexOf("/"));

        if (!path.endsWith(QLatin1Char('/')))
            path = path.append(QLatin1Char('/'));
        if (!path.startsWith(QLatin1Char('/')))
            path = path.prepend(QLatin1Char('/'));

        QString name{git_tree_entry_name(entry)};

        if (path.startsWith(w->prefix)) {
            qDebug() << "Path=" << path;
            auto newFilePath = w->destinationFolder + QLatin1Char('/') + path.mid(w->prefix.size()) + QLatin1Char('/') + name;
            QFileInfo fi{newFilePath};
            QDir d;
            d.mkpath(fi.absolutePath());

            auto file = w->tree->file(path + name);

            if (file.isNull()) {
                qDebug() << "File is null" << path << name;
                return 1;
            }
            return file.save(newFilePath) ? 0 : 1;
        }

        return 0;
    };

    wrapper w;
    w.destinationFolder = destinationFolder;
    w.tree = this;
    if (!prefix.startsWith(QLatin1Char('/')))
        w.prefix = QLatin1Char('/') + prefix;
    else
        w.prefix = prefix;

    return !git_tree_walk(d->tree, GIT_TREEWALK_PRE, cb, &w);
}

Oid Tree::oid() const
{
    return Oid{git_tree_id(d->tree)};
}

void TreePrivate::initTree()
{
    auto cb = [](const char *root, const git_tree_entry *entry, void *payload) -> int {
        auto w = reinterpret_cast<TreePrivate *>(payload);
        QString path{root};
        QString name{git_tree_entry_name(entry)};

        if (path.endsWith(QLatin1Char('/')))
            path = path.mid(0, path.length() - 1);

        auto type = git_tree_entry_type(entry);
        switch (type) {
        case GIT_OBJECT_BLOB:
            w->treeData.insert(path, TreeEntry{name, EntryType::File});
            w->entries.append(new TreeEntry{name, EntryType::File, path});
            break;
        case GIT_OBJECT_TREE:
            w->treeData.insert(path, TreeEntry{name, EntryType::Dir});
            break;
        case GIT_OBJECT_COMMIT:
        case GIT_OBJECT_ANY:
        case GIT_OBJECT_INVALID:
        case GIT_OBJECT_TAG:
            break;
        }
        return 0;
    };

    git_tree_walk(tree, GIT_TREEWALK_PRE, cb, this);
}

void TreePrivate::browseNestedEntities(EntryType type, const QString &path, QStringList &list) const
{
    QString prefix;
    if (!path.isEmpty())
        prefix = path + QLatin1Char('/');

    auto dirs = q->entries(path, EntryType::Dir);
    auto files = q->entries(path, EntryType::File);
    if (type == EntryType::Dir || type == EntryType::All) {
        for (auto const &dir : dirs)
            list.append(prefix + dir);
    }
    if (type == EntryType::File || type == EntryType::All) {
        for (const auto &f : files)
            list.append(prefix + f);
    }
    for (auto const &dir : dirs)
        browseNestedEntities(type, prefix + dir, list);
}

TreePrivate::TreePrivate(Tree *parent, git_tree *tree)
    : q{parent}
    , tree{tree}
{
}

TreePrivate::~TreePrivate()
{
    git_tree_free(tree);
}
EntryType TreeEntryLists::type(const QString &entryName) const
{
    auto i = std::find_if(mEntries.begin(), mEntries.end(), [&entryName](const Git::TreeEntry &en) {
        return en.name == entryName;
    });

    if (i == mEntries.end())
        return Git::EntryType::Unknown;

    return (*i).type;
}
bool TreeEntryLists::contains(const QString &entryName) const
{
    return std::any_of(mEntries.begin(), mEntries.end(), [&entryName](Git::TreeEntry en) {
        return en.name == entryName;
    });
}
qsizetype TreeEntryLists::indexOf(const QString &entryName, qsizetype from) const
{
    for (qsizetype i = from; i < mEntries.size(); ++i) {
        if (mEntries.at(i).name == entryName) {
            return i;
        }
    }

    return -1;
}
qsizetype TreeEntryLists::lastIndexOf(const QString &entryName, qsizetype from) const
{
    auto startIndex = from;
    if (startIndex == -1) {
        startIndex = mEntries.size();
    }

    for (qsizetype i = startIndex; i > 0; --i) {
        if (mEntries.at(i).name == entryName) {
            return i;
        }
    }

    return -1;
}
QList<TreeEntry>::iterator TreeEntryLists::begin()
{
    return mEntries.begin();
}
QList<TreeEntry>::iterator TreeEntryLists::end()
{
    return mEntries.end();
}
QList<TreeEntry>::const_iterator TreeEntryLists::begin() const noexcept
{
    return mEntries.begin();
}
QList<TreeEntry>::const_iterator TreeEntryLists::end() const noexcept
{
    return mEntries.end();
}
QList<TreeEntry>::const_iterator TreeEntryLists::cbegin() const noexcept
{
    return mEntries.cbegin();
}
QList<TreeEntry>::const_iterator TreeEntryLists::cend() const noexcept
{
    return mEntries.cend();
}
QList<TreeEntry>::const_iterator TreeEntryLists::constBegin() const noexcept
{
    return mEntries.constBegin();
}
QList<TreeEntry>::const_iterator TreeEntryLists::constEnd() const noexcept
{
    return mEntries.constEnd();
}
QList<TreeEntry>::reverse_iterator TreeEntryLists::rbegin()
{
    return mEntries.rbegin();
}
QList<TreeEntry>::reverse_iterator TreeEntryLists::rend()
{
    return mEntries.rend();
}
QList<TreeEntry>::const_reverse_iterator TreeEntryLists::rbegin() const noexcept
{
    return mEntries.rbegin();
}
QList<TreeEntry>::const_reverse_iterator TreeEntryLists::rend() const noexcept
{
    return mEntries.rend();
}
QList<TreeEntry>::const_reverse_iterator TreeEntryLists::crbegin() const noexcept
{
    return mEntries.crbegin();
}
QList<TreeEntry>::const_reverse_iterator TreeEntryLists::crend() const noexcept
{
    return mEntries.crend();
}
TreeEntry TreeEntryLists::at(qsizetype i)
{
    return mEntries.at(i);
}
qsizetype TreeEntryLists::size() const noexcept
{
    return mEntries.size();
}
qsizetype TreeEntryLists::count() const noexcept
{
    return mEntries.size();
}
qsizetype TreeEntryLists::length() const noexcept
{
    return mEntries.size();
}
TreeEntryLists::TreeEntryLists(const QList<TreeEntry> &mEntries)
    : mEntries(mEntries)
{
}

}

// QDebug operator<<(QDebug d, const QList<Git::TreeEntry> &list)
// {
//     bool first{true};

//     auto &dd = d.noquote().nospace();

//     for (auto &entry : list) {
//         if (!first)
//             dd << ", ";

//         dd << "(" << entry.name << ": ";

//         switch (entry.type) {
//         case Git::EntryType::Unknown:
//             dd << "Unknown";
//             break;
//         case Git::EntryType::File:
//             dd << "File";
//             break;
//         case Git::EntryType::Dir:
//             dd << "Dir";
//             break;
//         }
//         dd << ")";
//         first = false;
//     }

//     return dd;
// }
