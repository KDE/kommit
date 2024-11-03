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
    enum class EntryType {
        Unknown,
        File,
        Dir,
        All = Unknown
    };

    struct Entry {
        QString name;
        EntryType type;
        QString path{};
    };
    class EntryLists
    {
    public:
        explicit EntryLists(const QList<Entry> &mEntries);

        [[nodiscard]] EntryType type(const QString &entryName) const;
        [[nodiscard]] bool contains(const QString &entryName) const;
        [[nodiscard]] qsizetype indexOf(const QString &entryName, qsizetype from = 0) const;
        [[nodiscard]] qsizetype lastIndexOf(const QString &entryName, qsizetype from = -1) const;

        [[nodiscard]] inline QList<Entry>::iterator begin();
        [[nodiscard]] inline QList<Entry>::iterator end();

        [[nodiscard]] inline QList<Entry>::const_iterator begin() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_iterator end() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_iterator cbegin() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_iterator cend() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_iterator constBegin() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_iterator constEnd() const noexcept;
        [[nodiscard]] inline QList<Entry>::reverse_iterator rbegin();
        [[nodiscard]] inline QList<Entry>::reverse_iterator rend();
        [[nodiscard]] inline QList<Entry>::const_reverse_iterator rbegin() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_reverse_iterator rend() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_reverse_iterator crbegin() const noexcept;
        [[nodiscard]] inline QList<Entry>::const_reverse_iterator crend() const noexcept;

        Entry at(qsizetype i);
        [[nodiscard]] qsizetype size() const noexcept;
        [[nodiscard]] qsizetype count() const noexcept;
        [[nodiscard]] qsizetype length() const noexcept;

    private:
        QList<Entry> mEntries;
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

    [[nodiscard]] EntryLists entries(const QString &path) const;
    [[nodiscard]] QStringList entries(const QString &path, EntryType filter) const;
    [[nodiscard]] QStringList entries(EntryType filter) const;
    [[nodiscard]] Blob file(const QString &path) const;

    [[nodiscard]] bool extract(const QString &destinationFolder, const QString &prefix = {});
    [[nodiscard]] Oid oid() const override;

private:
    QSharedPointer<TreePrivate> d;
};

}
//
// template<>
// class LIBKOMMIT_EXPORT QListSpecialMethods<Git::Tree::Entry>
//{
// public:
//    Git::Tree::EntryType type(const QString &entryName) const;
//    bool contains(const QString &entryName) const;
//    qsizetype indexOf(const QString &entryName, qsizetype from = 0) const;
//    qsizetype lastIndexOf(const QString &entryName, qsizetype from = -1) const;
//};

QDebug LIBKOMMIT_EXPORT operator<<(QDebug d, const QList<Git::Tree::Entry> &list);
