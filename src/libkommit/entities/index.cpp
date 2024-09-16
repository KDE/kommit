/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/index.h"
#include "entities/strarray.h"
#include "gitglobal_p.h"
#include "types.h"

#include <git2/tree.h>

namespace Git
{

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

QList<ConflictIndex *> Index::conflicts() const
{
    QList<ConflictIndex *> list;

    git_index_conflict_iterator *it;
    git_index_conflict_iterator_new(&it, ptr);
    const git_index_entry *ancestorEntry;
    const git_index_entry *ourEntry;
    const git_index_entry *theirEntry;
    while (!git_index_conflict_next(&ancestorEntry, &ourEntry, &theirEntry, it)) {
        auto i = new ConflictIndex;
        i->ancestorIndex = new IndexEntry{ancestorEntry};
        i->ourIndex = new IndexEntry{ourEntry};
        i->theirIndex = new IndexEntry{theirEntry};
        list << i;
    }
    return list;
}

git_oid &Index::lastOid()
{
    return mLastOid;
}

IndexEntry::IndexEntry(const git_index_entry *entryPtr)
    : mEntryPtr{entryPtr}
{
}

QString IndexEntry::path() const
{
    return QString{mEntryPtr->path};
}

quint16 IndexEntry::fileSize() const
{
    return mEntryPtr->file_size;
}

QSharedPointer<Oid> IndexEntry::oid() const
{
    return QSharedPointer<Oid>{new Oid{&mEntryPtr->id}};
}

IndexEntry::StageState IndexEntry::stageState() const
{
    return static_cast<StageState>(git_index_entry_stage(mEntryPtr));
}

bool IndexEntry::isConflict() const
{
    return 1 == git_index_entry_is_conflict(mEntryPtr);
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
}
