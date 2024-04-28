/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tree.h"
#include "gitglobal.h"
#include "qdebug.h"
#include "types.h"

#include <git2/commit.h>
#include <git2/tree.h>

namespace Git
{

Tree::Tree(git_tree *tree)
    : ptr{tree}
{
    initTree();
}

Tree::~Tree()
{
    if (ptr)
        git_tree_free(ptr);
}

QList<Tree::Entry> Tree::entries(const QString &path) const
{
    return mTreeData.values(path);
}

QStringList Tree::entries(const QString &path, EntryType filter) const
{
    auto entries = this->entries(path);
    QStringList ret;
    for (auto &en : entries)
        if (filter == EntryType::All || en.type == filter)
            ret << en.name;
    return ret;
}

QStringList Tree::entries(EntryType filter) const
{
    QStringList list;

    browseNestedEntities(filter, "", list);

    return list;
}

QSharedPointer<File> Tree::file(const QString &path)
{
    git_tree_entry *entry;
    BEGIN
    STEP git_tree_entry_bypath(&entry, ptr, toConstChars(path));

    if (err)
        return {};

    return QSharedPointer<File>{new File{git_tree_owner(ptr), entry}};
}

git_tree *Tree::gitTree() const
{
    return ptr;
}

void Tree::initTree()
{
    auto cb = [](const char *root, const git_tree_entry *entry, void *payload) -> int {
        auto w = reinterpret_cast<Tree *>(payload);
        QString path{root};
        QString name{git_tree_entry_name(entry)};

        if (path.endsWith("/"))
            path = path.mid(0, path.length() - 1);

        auto type = git_tree_entry_type(entry);
        switch (type) {
        case GIT_OBJECT_BLOB:
            w->mTreeData.insert(path, Entry{name, EntryType::File});
            break;
        case GIT_OBJECT_TREE:
            w->mTreeData.insert(path, Entry{name, EntryType::Dir});
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

    git_tree_walk(ptr, GIT_TREEWALK_PRE, cb, this);
}

void Tree::browseNestedEntities(EntryType type, const QString &path, QStringList &list) const
{
    QString prefix;
    if (path != "")
        prefix = path + "/";

    auto dirs = entries(path, EntryType::Dir);
    auto files = entries(path, EntryType::File);
    if (type == EntryType::Dir || type == EntryType::All) {
        for (auto const &dir : dirs)
            list.append(prefix + dir);
    }
    if (type == EntryType::File || type == EntryType::All) {
        for (auto const &file : files)
            list.append(prefix + file);
    }
    for (auto const &dir : dirs)
        browseNestedEntities(type, prefix + dir, list);
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
