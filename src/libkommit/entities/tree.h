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

enum class EntryType {
    Unknown,
    File,
    Dir,
    All = Unknown
};

struct LIBKOMMIT_EXPORT TreeEntry {
    QString name;
    EntryType type;
    QString path{};
};

class LIBKOMMIT_EXPORT TreeEntryLists
{
public:
    explicit TreeEntryLists(const QList<TreeEntry> &mEntries);

    [[nodiscard]] EntryType type(const QString &entryName) const;
    [[nodiscard]] bool contains(const QString &entryName) const;
    [[nodiscard]] qsizetype indexOf(const QString &entryName, qsizetype from = 0) const;
    [[nodiscard]] qsizetype lastIndexOf(const QString &entryName, qsizetype from = -1) const;

    [[nodiscard]] inline QList<TreeEntry>::iterator begin();
    [[nodiscard]] inline QList<TreeEntry>::iterator end();

    [[nodiscard]] inline QList<TreeEntry>::const_iterator begin() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_iterator end() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_iterator cbegin() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_iterator cend() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_iterator constBegin() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_iterator constEnd() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::reverse_iterator rbegin();
    [[nodiscard]] inline QList<TreeEntry>::reverse_iterator rend();
    [[nodiscard]] inline QList<TreeEntry>::const_reverse_iterator rbegin() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_reverse_iterator rend() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_reverse_iterator crbegin() const noexcept;
    [[nodiscard]] inline QList<TreeEntry>::const_reverse_iterator crend() const noexcept;

    TreeEntry at(qsizetype i);
    [[nodiscard]] qsizetype size() const noexcept;
    [[nodiscard]] qsizetype count() const noexcept;
    [[nodiscard]] qsizetype length() const noexcept;

private:
    QList<TreeEntry> mEntries;
};

class LIBKOMMIT_EXPORT Tree : public IOid
{
public:
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

    [[nodiscard]] TreeEntryLists entries(const QString &path) const;
    [[nodiscard]] QStringList entries(const QString &path, EntryType filter) const;
    [[nodiscard]] QStringList entries(EntryType filter) const;
    [[nodiscard]] Blob file(const QString &path) const;

    [[nodiscard]] bool extract(const QString &destinationFolder, const QString &prefix = {});
    [[nodiscard]] Oid oid() const override;

private:
    QSharedPointer<TreePrivate> d;
};
}

// QDebug LIBKOMMIT_EXPORT operator<<(QDebug d, const QList<Git::TreeEntry> &list);
