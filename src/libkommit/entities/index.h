/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/index.h>
#include <git2/types.h>

#include <QString>

#include "entities/tree.h"
#include "libkommit_export.h"
#include "oid.h"

namespace Git
{

class LIBKOMMIT_EXPORT IndexEntry : IOid
{
public:
    enum class StageState { Normal, Base, Ours, Thers };

    IndexEntry(const git_index_entry *entryPtr);

    QString path() const;
    quint16 fileSize() const;
    QSharedPointer<Oid> oid() const override;
    StageState stageState() const;
    bool isConflict() const;

private:
    const git_index_entry *mEntryPtr;
};

struct LIBKOMMIT_EXPORT ConflictIndex {
    IndexEntry *ancestorIndex;
    IndexEntry *ourIndex;
    IndexEntry *theirIndex;
};

class LIBKOMMIT_EXPORT Index
{
public:
    class iterator
    {
    public:
        iterator(git_index *ptr);

        IndexEntry *operator*() const;

        bool operator==(const iterator &o) const noexcept;
        bool operator!=(const iterator &o) const noexcept;
        inline bool operator<(const iterator &other) const noexcept
        {
            return i < other.i;
        }
        inline bool operator<=(const iterator &other) const noexcept
        {
            return i <= other.i;
        }
        inline bool operator>(const iterator &other) const noexcept
        {
            return i > other.i;
        }
        inline bool operator>=(const iterator &other) const noexcept
        {
            return i >= other.i;
        }
        iterator &operator++();
        iterator operator++(int);

    private:
        git_index *const ptr;
        IndexEntry *t;
        int i;
    };

    explicit Index(git_index *index);
    ~Index();

    bool addAll();
    bool addByPath(const QString &path);
    bool removeByPath(const QString &path);
    bool removeAll();

    iterator begin();
    iterator end();

    bool write();
    bool writeTree();
    QSharedPointer<Tree> tree() const;

    QList<IndexEntry *> entries() const;
    QList<ConflictIndex *> conflicts() const;
    git_oid &lastOid();

private:
    git_index *const ptr;
    git_oid mLastOid;
    bool writeNeeded{false};
};

}
