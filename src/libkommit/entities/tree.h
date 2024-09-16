/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "interfaces.h"
#include "libkommit_export.h"

#include <git2/types.h>

#include <QDebug>
#include <QList>
#include <QMultiMap>
#include <QString>

namespace Git
{

class Blob;

class TreePrivate;
class LIBKOMMIT_EXPORT Tree : public IOid
{
public:
    enum class EntryType { Unknown, File, Dir, All = Unknown };

    struct Entry {
        QString name;
        EntryType type;
        QString path;
    };

    explicit Tree(git_tree *tree);
    Tree(git_repository *repo, const QString &place);
    ~Tree();

    [[nodiscard]] QList<Entry> entries(const QString &path) const;
    [[nodiscard]] QStringList entries(const QString &path, EntryType filter) const;
    [[nodiscard]] QStringList entries(EntryType filter) const;
    QSharedPointer<Blob> file(const QString &path);

    [[nodiscard]] git_tree *gitTree() const;
    [[nodiscard]] bool extract(const QString &destinationFolder, const QString &prefix = {});
    QSharedPointer<Oid> oid() const override;

private:
    TreePrivate *const d_ptr;
    Q_DECLARE_PRIVATE(Tree)
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
