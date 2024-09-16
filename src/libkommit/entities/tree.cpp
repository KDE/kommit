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
    TreePrivate(Tree *parent, git_tree *tree);
    Tree *q_ptr;
    Q_DECLARE_PUBLIC(Tree)

    QList<Tree::Entry *> entries;
    QMultiMap<QString, Tree::Entry> treeData;

    git_tree *gitTreePtr{nullptr};

    void initTree();
    void browseNestedEntities(Tree::EntryType type, const QString &path, QStringList &list) const;
};

Tree::Tree(git_tree *tree)
    : d_ptr{new TreePrivate{this, tree}}
{
    Q_D(Tree);
    d->initTree();
}

Tree::Tree(git_repository *repo, const QString &place)
    : d_ptr{new TreePrivate{this, nullptr}}
{
    git_tree *tree;
    git_object *placeObject{nullptr};
    git_commit *commit{nullptr};

    BEGIN
    STEP git_revparse_single(&placeObject, repo, place.toLatin1().constData());
    STEP git_commit_lookup(&commit, repo, git_object_id(placeObject));
    STEP git_commit_tree(&tree, commit);

    if (IS_OK) {
        Q_D(Tree);
        d->gitTreePtr = tree;
        d->initTree();
    }
}

Tree::~Tree()
{
    Q_D(Tree);
    if (d->gitTreePtr)
        git_tree_free(d->gitTreePtr);
    delete d;
}

QList<Tree::Entry> Tree::entries(const QString &path) const
{
    Q_D(const Tree);
    return d->treeData.values(path);
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
    Q_D(const Tree);

    QStringList list;

    d->browseNestedEntities(filter, QString(), list);

    return list;
}

QSharedPointer<Blob> Tree::file(const QString &path)
{
    Q_D(Tree);
    git_tree_entry *entry;
    BEGIN

    if (path.startsWith(QLatin1Char('/')))
        STEP git_tree_entry_bypath(&entry, d->gitTreePtr, toConstChars(path.mid(1)));
    else
        STEP git_tree_entry_bypath(&entry, d->gitTreePtr, toConstChars(path));

    if (IS_ERROR)
        return {};

    return QSharedPointer<Blob>{new Blob{git_tree_owner(d->gitTreePtr), entry}};
}

git_tree *Tree::gitTree() const
{
    Q_D(const Tree);
    return d->gitTreePtr;
}

bool Tree::extract(const QString &destinationFolder, const QString &prefix)
{
    Q_D(Tree);
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
            QSharedPointer<Blob> file;

            file = w->tree->file(path + name);

            if (file.isNull()) {
                qDebug() << "File is null" << path << name;
                return 1;
            }
            return file->save(newFilePath) ? 0 : 1;
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

    return !git_tree_walk(d->gitTreePtr, GIT_TREEWALK_PRE, cb, &w);
}

QSharedPointer<Oid> Tree::oid() const
{
    Q_D(const Tree);
    return QSharedPointer<Oid>{new Oid{git_tree_id(d->gitTreePtr)}};
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
            w->treeData.insert(path, Tree::Entry{name, Tree::EntryType::File});
            w->entries.append(new Tree::Entry{name, Tree::EntryType::File, path});
            break;
        case GIT_OBJECT_TREE:
            w->treeData.insert(path, Tree::Entry{name, Tree::EntryType::Dir});
            break;
        case GIT_OBJECT_COMMIT:
        case GIT_OBJECT_ANY:
        case GIT_OBJECT_INVALID:
        case GIT_OBJECT_TAG:
        case GIT_OBJECT_OFS_DELTA:
        case GIT_OBJECT_REF_DELTA:
            break;
        }
        return 0;
    };

    git_tree_walk(gitTreePtr, GIT_TREEWALK_PRE, cb, this);
}

void TreePrivate::browseNestedEntities(Tree::EntryType type, const QString &path, QStringList &list) const
{
    Q_Q(const Tree);
    QString prefix;
    if (!path.isEmpty())
        prefix = path + QLatin1Char('/');

    auto dirs = q->entries(path, Tree::EntryType::Dir);
    auto files = q->entries(path, Tree::EntryType::File);
    if (type == Tree::EntryType::Dir || type == Tree::EntryType::All) {
        for (auto const &dir : dirs)
            list.append(prefix + dir);
    }
    if (type == Tree::EntryType::File || type == Tree::EntryType::All) {
        for (const auto &f : files)
            list.append(prefix + f);
    }
    for (auto const &dir : dirs)
        browseNestedEntities(type, prefix + dir, list);
}

TreePrivate::TreePrivate(Tree *parent, git_tree *tree)
    : q_ptr{parent}
    , gitTreePtr{tree}
{
}
}

Git::Tree::EntryType QListSpecialMethods<Git::Tree::Entry>::type(const QString &entryName) const
{
    auto self = static_cast<QList<Git::Tree::Entry> *>(const_cast<QListSpecialMethods<Git::Tree::Entry> *>(this));

    auto i = std::find_if(self->begin(), self->end(), [&entryName](Git::Tree::Entry en) {
        return en.name == entryName;
    });

    if (i == self->end())
        return Git::Tree::EntryType::Unknown;

    return (*i).type;
}

QDebug operator<<(QDebug d, const QList<Git::Tree::Entry> &list)
{
    bool first{true};

    auto &dd = d.noquote().nospace();

    for (auto &entry : list) {
        if (!first)
            dd << ", ";

        dd << "(" << entry.name << ": ";

        switch (entry.type) {
        case Git::Tree::EntryType::Unknown:
            dd << "Unknown";
            break;
        case Git::Tree::EntryType::File:
            dd << "File";
            break;
        case Git::Tree::EntryType::Dir:
            dd << "Dir";
            break;
        }
        dd << ")";
        first = false;
    }

    return dd;
}

bool QListSpecialMethods<Git::Tree::Entry>::contains(const QString &entryName) const
{
    auto self = static_cast<QList<Git::Tree::Entry> *>(const_cast<QListSpecialMethods<Git::Tree::Entry> *>(this));

    return std::any_of(self->begin(), self->end(), [&entryName](Git::Tree::Entry en) {
        return en.name == entryName;
    });
}

qsizetype QListSpecialMethods<Git::Tree::Entry>::indexOf(const QString &entryName, qsizetype from) const
{
    auto self = static_cast<QList<Git::Tree::Entry> *>(const_cast<QListSpecialMethods<Git::Tree::Entry> *>(this));

    for (qsizetype i = 0; i < self->size(); ++i) {
        if (self->at(i).name == entryName) {
            return i;
        }
    }

    return -1;
}

qsizetype QListSpecialMethods<Git::Tree::Entry>::lastIndexOf(const QString &entryName, qsizetype from) const
{
    auto self = static_cast<QList<Git::Tree::Entry> *>(const_cast<QListSpecialMethods<Git::Tree::Entry> *>(this));

    auto startIndex = from;
    if (startIndex == -1) {
        startIndex = self->size();
    }

    for (qsizetype i = startIndex; i > 0; --i) {
        if (self->at(i).name == entryName) {
            return i;
        }
    }

    return -1;
}
