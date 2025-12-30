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

#include "libkommit_export.h"
#include "libkommit_global.h"
#include "types.h"

#include <Kommit/Blob>
#include <Kommit/FileMode>
#include <Kommit/ITree>
#include <Kommit/Oid>
#include <Kommit/Tree>

namespace Git
{

class Blob;

class IndexEntryPrivate;
class LIBKOMMIT_EXPORT IndexEntry : IOid
{
public:
    enum class StageState {
        Normal,
        Base,
        Ours,
        Thers
    };

    IndexEntry();
    explicit IndexEntry(const git_index_entry *entryPtr);

    QString path() const;
    quint16 fileSize() const;
    Git::Oid oid() const override;
    StageState stageState() const;
    bool isConflict() const;
    Git::FileMode mode() const;

private:
    QSharedPointer<IndexEntryPrivate> d;
};

class LIBKOMMIT_EXPORT IndexConflictEntry : public IndexEntry
{
public:
    IndexConflictEntry(git_repository *repo, const git_index_entry *entryPtr);

    [[nodiscard]] const Blob &blob() const;

private:
    Blob mBlob;
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

class IndexPrivate;
class LIBKOMMIT_EXPORT Index : public ITree
{
public:
    Index();
    explicit Index(git_index *index);

    Index(const Index &other);
    Index &operator=(const Index &other);
    bool operator==(const Index &other) const;
    bool operator!=(const Index &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_index *data() const;
    [[nodiscard]] const git_index *constData() const;

    bool addAll();
    bool addByPath(const QString &path);
    bool removeByPath(const QString &path);
    bool removeAll();

    bool write();
    bool writeTree();

    QString treeTitle() const override;
    Tree tree() const override;

    QList<IndexEntry> entries() const;
    IndexEntry entryByPath(const QString &path) const;
    Blob blobByPath(const QString &path) const;

    bool hasConflicts() const;
    QSharedPointer<ConflictIndexList> conflicts() const;
    git_oid &lastOid();

    void resolveConflict(ConflictIndex *conflict, ConflictSide side);

private:
    QSharedPointer<IndexPrivate> d;
};
}
