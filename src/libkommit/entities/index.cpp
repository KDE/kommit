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

class IndexEntryPrivate
{
public:
    explicit IndexEntryPrivate(const git_index_entry *entry);
    const git_index_entry *entry;
    QString path;
    quint16 fileSize;
    Git::Oid oid;
    IndexEntry::StageState stageState;
    bool isConflict;
    Git::FileMode fileMode;
};

IndexEntryPrivate::IndexEntryPrivate(const git_index_entry *entry)
    : entry{entry}
{
    if (entry) {
        oid = Oid{entry->id};
        path = QString{entry->path};
        fileSize = entry->file_size;
        fileMode = entry->mode;
        stageState = static_cast<IndexEntry::StageState>(git_index_entry_stage(entry));
        isConflict = 1 == git_index_entry_is_conflict(entry);
    }
}

IndexEntry::IndexEntry()
    : d{new IndexEntryPrivate{nullptr}}
{
}

IndexEntry::IndexEntry(const git_index_entry *entryPtr)
    : d{new IndexEntryPrivate{entryPtr}}
{
}

QString IndexEntry::path() const
{
    return d->path;
}
quint16 IndexEntry::fileSize() const
{
    return d->fileSize;
}
Git::Oid IndexEntry::oid() const
{
    return d->oid;
}
IndexEntry::StageState IndexEntry::stageState() const
{
    return d->stageState;
}
bool IndexEntry::isConflict() const
{
    return d->isConflict;
}
Git::FileMode IndexEntry::mode() const
{
    return d->fileMode;
}

class IndexPrivate
{
public:
    explicit IndexPrivate(git_index *index = nullptr);
    ~IndexPrivate();
    git_index *index{nullptr};
    git_oid mLastOid;
    bool writeNeeded{false};
};

IndexPrivate::IndexPrivate(git_index *index)
    : index{index}
{
}

IndexPrivate::~IndexPrivate()
{
    git_index_free(index);
}

Index::Index()
    : d{new IndexPrivate{nullptr}}
{
}

Index::Index(git_index *index)
    : d{new IndexPrivate{index}}
{
}

Index::~Index()
{
    git_index_free(d->index);
}

Index::Index(const Index &other)
    : d{other.d}
{
}

Index &Index::operator=(const Index &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Index::operator==(const Index &other) const
{
    return d->index == other.d->index;
}

bool Index::operator!=(const Index &other) const
{
    return !(*this == other);
}

bool Index::isNull() const
{
    return !d->index;
}

git_index *Index::data() const
{
    return d->index;
}

const git_index *Index::constData() const
{
    return d->index;
}

bool Index::addAll()
{
    git_strarray a;
    addToArray(&a, QStringLiteral("/"));
    if (git_index_add_all(d->index, &a, GIT_INDEX_ADD_FORCE, NULL, nullptr)) {
        return false;
    } else {
        d->writeNeeded = true;
        return true;
    }
}

bool Index::addByPath(const QString &path)
{
    BEGIN;
    if (path.startsWith(QLatin1Char('/')))
        STEP git_index_add_bypath(d->index, toConstChars(path.mid(1)));
    else
        STEP git_index_add_bypath(d->index, path.toUtf8().data());
    PRINT_ERROR;

    if (IS_OK)
        d->writeNeeded = true;

    return IS_OK;
}

bool Index::removeByPath(const QString &path)
{
    BEGIN;
    STEP git_index_remove_bypath(d->index, toConstChars(path));
    if (IS_OK)
        d->writeNeeded = true;

    return IS_OK;
}

bool Index::removeAll()
{
    git_strarray arr;
    addToArray(&arr, QStringLiteral("/"));
    if (git_index_remove_all(d->index, &arr, NULL, NULL)) {
        return false;
    } else {
        d->writeNeeded = true;
        return true;
    }
}

bool Index::write()
{
    return !git_index_write(d->index);
}

bool Index::writeTree()
{
    if (!d->writeNeeded)
        return true;
    git_index_write(d->index);
    return !git_index_write_tree(&d->mLastOid, d->index);
}

QString Index::treeTitle() const
{
    return QString{};
}

Tree Index::tree() const
{
    git_tree *tree;
    git_oid oid;

    auto repo = git_index_owner(d->index);
    if (git_tree_lookup(&tree, repo, &oid))
        return Tree{};

    return Tree{tree};
}

QList<IndexEntry> Index::entries() const
{
    QList<IndexEntry> list;

    auto entryCount = git_index_entrycount(d->index);
    for (size_t i = 0; i < entryCount; i++) {
        auto entry = git_index_get_byindex(d->index, i);

        if (entry == NULL)
            continue;

        list << IndexEntry{entry};
    }
    return list;
}

IndexEntry Index::entryByPath(const QString &path) const
{
    auto entry = git_index_get_bypath(d->index, path.toUtf8().data(), 0);
    if (entry)
        return IndexEntry{entry};

    return IndexEntry{};
}

Blob Index::blobByPath(const QString &path) const
{
    auto entry = git_index_get_bypath(d->index, path.toUtf8().data(), 0);
    if (!entry)
        return Blob{};

    return Blob{git_index_owner(d->index), entry};
}

bool Index::hasConflicts() const
{
    return 1 == git_index_has_conflicts(d->index);
}

QSharedPointer<ConflictIndexList> Index::conflicts() const
{
    QList<ConflictIndex *> list;

    git_index_conflict_iterator *it;
    git_index_conflict_iterator_new(&it, d->index);
    const git_index_entry *ancestorEntry;
    const git_index_entry *ourEntry;
    const git_index_entry *theirEntry;
    auto repo = git_index_owner(d->index);

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
    return d->mLastOid;
}

IndexConflictEntry::IndexConflictEntry(git_repository *repo, const git_index_entry *entryPtr)
    : IndexEntry{entryPtr}
{
    if (entryPtr)
        mBlob = Blob{repo, entryPtr};
}

const Blob &IndexConflictEntry::blob() const
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
