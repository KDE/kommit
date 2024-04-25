/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "file.h"
#include "libkommit_export.h"

#include <git2/types.h>

#include <QDebug>
#include <QList>
#include <QMultiMap>
#include <QString>

namespace Git
{

class Commit;
class LIBKOMMIT_EXPORT Tree
{
public:
    enum class EntryType { Unknown, File, Dir, All = Unknown };

    struct Entry {
        QString name;
        EntryType type;
    };

    Tree(git_tree *tree);
    ~Tree();

    QList<Entry> entries(const QString &path) const;
    QStringList entries(const QString &path, EntryType filter) const;
    QStringList entries(EntryType filter) const;
    QSharedPointer<File> file(const QString &path);

    git_tree *gitTree() const;

private:
    git_tree *ptr{nullptr};
    void initTree();
    QMultiMap<QString, Entry> mTreeData;
    void browseNestedEntities(EntryType type, const QString &path, QStringList &list) const;
};

}

template<>
class LIBKOMMIT_EXPORT QListSpecialMethods<Git::Tree::Entry>
{
public:
    Git::Tree::EntryType type(const QString &entryName) const;
    bool contains(const QString &entryName) const;
    qsizetype indexOf(const QString &entryName, qsizetype from = 0) const;
    qsizetype lastIndexOf(const QString &entryName, qsizetype from = -1) const;
};

QDebug LIBKOMMIT_EXPORT operator<<(QDebug d, const QList<Git::Tree::Entry> &list);
