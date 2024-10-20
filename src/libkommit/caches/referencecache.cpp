/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "referencecache.h"

#include "branch.h"
#include "caches/commitscache.h"
#include "entities/commit.h"
#include "entities/oid.h"
#include "entities/remote.h"
#include "entities/tag.h"
#include "gitglobal_p.h"
#include "repository.h"

#include <git2/refs.h>

#include <QDebug>

namespace Git
{

ReferenceCache::ReferenceCache(Repository *parent)
    : Git::Cache<Reference, git_reference>{parent}
    , d_ptr{new ReferenceCachePrivate{this}}
{
}

ReferenceCache::~ReferenceCache()
{
}

ReferenceCache::DataType ReferenceCache::findByName(const QString &name)
{
    git_reference *ref;

    BEGIN;
    STEP git_reference_lookup(&ref, manager->repoPtr(), name.toUtf8().data());

    if (IS_ERROR)
        return DataType{};

    return findByPtr(ref);
}

ReferenceCache::DataType ReferenceCache::findForNote(const Note &note)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&note](DataType c) {
        return c.isNote() && c.toNote() == note;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataType ReferenceCache::findForBranch(const Branch &branch)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&branch](DataType c) {
        return c.isBranch() && c.toBranch() == branch;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataType ReferenceCache::findForTag(const Tag &tag)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&tag](DataType c) {
        return c.isTag() && c.toTag() == tag;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataType ReferenceCache::findForRemote(const Remote &remote)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&remote](DataType c) {
        return c.isRemote() && c.toRemote() == remote;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::ListType ReferenceCache::findForCommit(const Commit &commit)
{
    Q_D(ReferenceCache);
    if (!mList.size())
        fill();

    return d->dataByCommit.values(commit);
}

void ReferenceCache::forEach(std::function<void(DataType)> callback) const
{
    struct wrapper {
        std::function<void(const Reference &)> cb;
    };
    auto cb = [](git_reference *reference, void *payload) -> int {
        auto w = reinterpret_cast<wrapper *>(payload);
        w->cb(Reference{reference});
        return 0;
    };

    wrapper w;
    w.cb = callback;
    git_reference_foreach(manager->repoPtr(), cb, &w);
}

void ReferenceCache::clearChildData()
{
    Q_D(ReferenceCache);
    d->list.clear();
    d->dataByCommit.clear();
}

void ReferenceCache::fill()
{
    Q_D(ReferenceCache);
    d->fill();
}

ReferenceCachePrivate::ReferenceCachePrivate(ReferenceCache *parent)
    : q_ptr{parent}
{
}

void ReferenceCachePrivate::fill()
{
    Q_Q(ReferenceCache);

    git_reference_iterator *iterator;
    git_reference *reference;
    BEGIN;
    STEP git_reference_iterator_new(&iterator, q->manager->repoPtr());

    if (IS_ERROR)
        return;

    while (!git_reference_next(&reference, iterator)) {
        auto ref = q->findByPtr(reference);
        list << ref;
        auto hash = ref.target().toString();
        auto commit = q->manager->commits()->find(hash);
        if (Q_LIKELY(!commit.isNull())) // TODO: check if is this possible?
            dataByCommit.insert(commit, ref);
    }

    git_reference_iterator_free(iterator);
}
}
