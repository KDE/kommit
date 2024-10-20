/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/index.h>
#include <git2/types.h>

#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>

#include "entities/tree.h"
#include "libkommit_export.h"
#include "libkommit_global.h"
#include "oid.h"
#include "types.h"
#include <Kommit/FileMode>

namespace Git
{

class Blob;
class IndexEntryPrivate;
class LIBKOMMIT_EXPORT IndexEntry : IOid
{
public:
    enum class StageState { Normal, Base, Ours, Thers };

    IndexEntry(const git_index_entry *entryPtr);
    ~IndexEntry();

    QString path() const;
    quint16 fileSize() const;
    QSharedPointer<Oid> oid() const override;
    StageState stageState() const;
    bool isConflict() const;
    FileMode mode() const;

private:
    QScopedPointer<IndexEntryPrivate> d_ptr;
    Q_DECLARE_PRIVATE(IndexEntry)
};

class IndexEntryPrivate
{
    IndexEntry *q_ptr;
    Q_DECLARE_PUBLIC(IndexEntry)

public:
    IndexEntryPrivate(IndexEntry *parent, const git_index_entry *entry);

    QString path;
    quint16 fileSize;
    QSharedPointer<Oid> oid;
    IndexEntry::StageState stageState;
    bool isConflict;
    FileMode fileMode;
};

class LIBKOMMIT_EXPORT IndexConflictEntry : public IndexEntry
{
public:
    IndexConflictEntry(git_repository *repo, const git_index_entry *entryPtr);

    [[nodiscard]] QSharedPointer<Blob> blob() const;

private:
    QSharedPointer<Blob> mBlob;
};

struct LIBKOMMIT_EXPORT ConflictIndex {
    QScopedPointer<IndexConflictEntry> ancestorIndex{nullptr};
    QScopedPointer<IndexConflictEntry> ourIndex{nullptr};
    QScopedPointer<IndexConflictEntry> theirIndex{nullptr};
};

class ConflictIndexListPrivate;
class LIBKOMMIT_EXPORT ConflictIndexList
{
public:
    using ListType = QList<ConflictIndex *>;

    ConflictIndexList();
    ConflictIndexList(ListType list);
    ~ConflictIndexList();

    [[nodiscard]] qsizetype size() const;
    [[nodiscard]] ListType conflicts() const;
    [[nodiscard]] ConflictIndex *at(qsizetype index) const;
    [[nodiscard]] ListType::const_iterator begin() const;
    [[nodiscard]] ListType::const_iterator end() const;

private:
    QScopedPointer<ConflictIndexListPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ConflictIndexList)
};

class ConflictIndexListPrivate
{
    ConflictIndexList *q_ptr;
    Q_DECLARE_PUBLIC(ConflictIndexList)

public:
    explicit ConflictIndexListPrivate(ConflictIndexList *parent, QList<ConflictIndex *> conflicts);
    ~ConflictIndexListPrivate();

    QList<ConflictIndex *> conflicts;
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
    IndexEntry *entryByPath(const QString &path) const;
    QSharedPointer<Blob> blobByPath(const QString &path) const;

    bool hasConflicts() const;
    QSharedPointer<ConflictIndexList> conflicts() const;
    git_oid &lastOid();

    void resolveConflict(ConflictIndex *conflict, ConflictSide side);

private:
    git_index *const ptr;
    git_oid mLastOid;
    bool writeNeeded{false};
};

}
