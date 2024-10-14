/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/index.h"
#include "blob.h"
#include "entities/strarray.h"
#include "gitglobal_p.h"
#include "types.h"

#include <git2/tree.h>

#include <Kommit/Repository>

namespace Git
{

IndexEntryPrivate::IndexEntryPrivate(IndexEntry *parent, const git_index_entry *entry)
    : q_ptr{parent}
{
    if (entry) {
        oid.reset(new Oid{entry->id});
        path = QString{entry->path};
        fileSize = entry->file_size;
        fileMode = entry->mode;
        stageState = static_cast<IndexEntry::StageState>(git_index_entry_stage(entry));
        isConflict = 1 == git_index_entry_is_conflict(entry);
    }
}

Index::Index(git_index *index)
    : ptr{index}
{
}

Index::~Index()
{
    git_index_free(ptr);
}

bool Index::addAll()
{
    git_strarray a;
    addToArray(&a, QStringLiteral("/"));
    if (git_index_add_all(ptr, &a, GIT_INDEX_ADD_FORCE, NULL, nullptr)) {
        return false;
    } else {
        writeNeeded = true;
        return true;
    }
}

bool Index::addByPath(const QString &path)
{
    BEGIN;
    if (path.startsWith(QLatin1Char('/')))
        STEP git_index_add_bypath(ptr, toConstChars(path.mid(1)));
    else
        STEP git_index_add_bypath(ptr, path.toUtf8().data());
    PRINT_ERROR;

    if (IS_OK)
        writeNeeded = true;

    return IS_OK;
}

bool Index::removeByPath(const QString &path)
{
    BEGIN;
    STEP git_index_remove_bypath(ptr, toConstChars(path));
    if (IS_OK)
        writeNeeded = true;

    return IS_OK;
}

bool Index::removeAll()
{
    git_strarray arr;
    addToArray(&arr, QStringLiteral("/"));
    if (git_index_remove_all(ptr, &arr, NULL, NULL)) {
        return false;
    } else {
        writeNeeded = true;
        return true;
    }
}

Index::iterator Index::begin()
{
    return iterator{ptr};
}

Index::iterator Index::end()
{
    return iterator{ptr};
}

bool Index::write()
{
    return !git_index_write(ptr);
}

bool Index::writeTree()
{
    if (!writeNeeded)
        return true;
    git_index_write(ptr);
    return !git_index_write_tree(&mLastOid, ptr);
}

QSharedPointer<Tree> Index::tree() const
{
    git_tree *tree;
    git_oid oid;

    auto repo = git_index_owner(ptr);
    if (git_tree_lookup(&tree, repo, &oid))
        return nullptr;

    return QSharedPointer<Tree>{new Tree{tree}};
}

QList<IndexEntry *> Index::entries() const
{
    QList<IndexEntry *> list;

    auto entryCount = git_index_entrycount(ptr);
    for (size_t i = 0; i < entryCount; i++) {
        auto entry = git_index_get_byindex(ptr, i);

        if (entry == NULL)
            continue;

        list << new IndexEntry{entry};
    }
    return list;
}

IndexEntry *Index::entryByPath(const QString &path) const
{
    auto entry = git_index_get_bypath(ptr, path.toUtf8().data(), 0);
    if (entry)
        new IndexEntry{entry};

    return nullptr;
}

QSharedPointer<Blob> Index::blobByPath(const QString &path) const
{
    auto entry = git_index_get_bypath(ptr, path.toUtf8().data(), 0);
    if (!entry)
        return {};

    return QSharedPointer<Blob>::create(git_index_owner(ptr), entry);
}

bool Index::hasConflicts() const
{
    return 1 == git_index_has_conflicts(ptr);
}

QSharedPointer<ConflictIndexList> Index::conflicts() const
{
    QList<ConflictIndex *> list;

    git_index_conflict_iterator *it;
    git_index_conflict_iterator_new(&it, ptr);
    const git_index_entry *ancestorEntry;
    const git_index_entry *ourEntry;
    const git_index_entry *theirEntry;
    auto repo = git_index_owner(ptr);

    while (!git_index_conflict_next(&ancestorEntry, &ourEntry, &theirEntry, it)) {
        auto i = new ConflictIndex;
        i->ancestorIndex.reset(new IndexConflictEntry{repo, ancestorEntry});
        i->ourIndex.reset(new IndexConflictEntry{repo, ourEntry});
        i->theirIndex.reset(new IndexConflictEntry{repo, theirEntry});
        list << i;
    }
    return QSharedPointer<ConflictIndexList>::create(list);
}

git_oid &Index::lastOid()
{
    return mLastOid;
}

IndexEntry::IndexEntry(const git_index_entry *entryPtr)
    : d_ptr{new IndexEntryPrivate{this, entryPtr}}
{
}

IndexEntry::~IndexEntry()
{
}

QString IndexEntry::path() const
{
    Q_D(const IndexEntry);
    return d->path;
}

quint16 IndexEntry::fileSize() const
{
    Q_D(const IndexEntry);
    return d->fileSize;
}

QSharedPointer<Oid> IndexEntry::oid() const
{
    Q_D(const IndexEntry);
    return d->oid;
}

IndexEntry::StageState IndexEntry::stageState() const
{
    Q_D(const IndexEntry);
    return d->stageState;
}

bool IndexEntry::isConflict() const
{
    Q_D(const IndexEntry);
    return d->isConflict;
}

FileMode IndexEntry::mode() const
{
    Q_D(const IndexEntry);
    return d->fileMode;
}

Index::iterator::iterator(git_index *ptr)
    : ptr(ptr)
{
}

IndexEntry *Index::iterator::operator*() const
{
    return new IndexEntry{git_index_get_byindex(ptr, i)};
}

bool Index::iterator::operator==(const iterator &o) const noexcept
{
    return i == o.i && ptr == o.ptr;
}

bool Index::iterator::operator!=(const iterator &o) const noexcept
{
    return !(*this == o);
}

Index::iterator &Index::iterator::operator++()
{
    ++i;
    return *this;
}

Index::iterator Index::iterator::operator++(int)
{
    ++i;
    return *this;
}

IndexConflictEntry::IndexConflictEntry(git_repository *repo, const git_index_entry *entryPtr)
    : IndexEntry{entryPtr}
{
    if (entryPtr)
        mBlob.reset(new Blob{repo, entryPtr});
}

QSharedPointer<Blob> IndexConflictEntry::blob() const
{
    return mBlob;
}

ConflictIndexListPrivate::ConflictIndexListPrivate(ConflictIndexList *parent, QList<ConflictIndex *> conflicts)
    : q_ptr{parent}
    , conflicts{conflicts}
{
}

ConflictIndexListPrivate::~ConflictIndexListPrivate()
{
    qDeleteAll(conflicts);
}

ConflictIndexList::ConflictIndexList()
    : d_ptr{new ConflictIndexListPrivate{this, {}}}
{
}

ConflictIndexList::ConflictIndexList(ListType list)
    : d_ptr{new ConflictIndexListPrivate{this, list}}
{
}

ConflictIndexList::~ConflictIndexList()
{
}

qsizetype ConflictIndexList::size() const
{
    Q_D(const ConflictIndexList);
    return d->conflicts.size();
}

ConflictIndexList::ListType ConflictIndexList::conflicts() const
{
    Q_D(const ConflictIndexList);
    return d->conflicts;
}

ConflictIndex *ConflictIndexList::at(qsizetype index) const
{
    Q_D(const ConflictIndexList);
    return d->conflicts.at(index);
}

ConflictIndexList::ListType::const_iterator ConflictIndexList::begin() const
{
    Q_D(const ConflictIndexList);
    return d->conflicts.begin();
}

ConflictIndexList::ListType::const_iterator ConflictIndexList::end() const
{
    Q_D(const ConflictIndexList);
    return d->conflicts.end();
}
}
