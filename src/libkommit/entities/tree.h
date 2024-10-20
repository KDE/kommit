/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/types.h>

#include <QDebug>
#include <QList>
#include <QMultiMap>
#include <QSharedPointer>
#include <QString>

#include <Kommit/Blob>
#include <Kommit/IOid>

namespace Git
{

class TreePrivate;
class LIBKOMMIT_EXPORT Tree : public IOid
{
public:
    enum class EntryType { Unknown, File, Dir, All = Unknown };

    struct Entry {
        QString name;
        EntryType type;
        QString path{};
    };

    Tree();
    explicit Tree(git_tree *tree);
    Tree(git_repository *repo, const QString &place);
    Tree(const Tree &other);

    Tree &operator=(const Tree &other);
    bool operator==(const Tree &other) const;
    bool operator!=(const Tree &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_tree *data() const;

    [[nodiscard]] const git_tree *constData() const;

    [[nodiscard]] QList<Entry> entries(const QString &path) const;
    [[nodiscard]] QStringList entries(const QString &path, EntryType filter) const;
    [[nodiscard]] QStringList entries(EntryType filter) const;
    Blob file(const QString &path) const;

    [[nodiscard]] git_tree *gitTree() const;
    [[nodiscard]] bool extract(const QString &destinationFolder, const QString &prefix = {});
    Oid oid() const override;

private:
    QSharedPointer<TreePrivate> d;
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
