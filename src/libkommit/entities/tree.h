/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "file.h"
#include "interfaces.h"
#include "libkommit_export.h"

#include <git2/types.h>

#include <QDebug>
#include <QList>
#include <QMultiMap>
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Tree : public IOid
{
public:
    enum class EntryType { Unknown, File, Dir, All = Unknown };

    struct Entry {
        QString name;
        EntryType type;
    };

    explicit Tree(git_tree *tree);
    ~Tree();

    Q_REQUIRED_RESULT QList<Entry> entries(const QString &path) const;
    Q_REQUIRED_RESULT QStringList entries(const QString &path, EntryType filter) const;
    Q_REQUIRED_RESULT QStringList entries(EntryType filter) const;
    QSharedPointer<File> file(const QString &path);

    Q_REQUIRED_RESULT git_tree *gitTree() const;
    Q_REQUIRED_RESULT bool extract(const QString &destinationFolder, const QString &prefix = {});
    QSharedPointer<Oid> oid() const override;

private:
    LIBKOMMIT_NO_EXPORT void initTree();
    LIBKOMMIT_NO_EXPORT void browseNestedEntities(EntryType type, const QString &path, QStringList &list) const;
    git_tree *ptr{nullptr};
    QMultiMap<QString, Entry> mTreeData;
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
