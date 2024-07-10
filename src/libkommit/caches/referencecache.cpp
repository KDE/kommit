/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "referencecache.h"

#include "entities/commit.h"
#include "entities/oid.h"
#include "entities/reference.h"
#include "entities/remote.h"
#include "gitglobal_p.h"
#include "gitmanager.h"

#include <git2/refs.h>

#include <QDebug>
#include <iostream>

namespace Git
{

ReferenceCache::ReferenceCache(Manager *parent)
    : Git::Cache<Reference, git_reference>{parent}
{
}

ReferenceCache::DataMember ReferenceCache::findForNote(QSharedPointer<Note> note)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&note](DataMember c) {
        return c->isNote() && c->toNote() == note;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataMember ReferenceCache::findForBranch(QSharedPointer<Branch> branch)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&branch](DataMember c) {
        return c->isBranch() && c->toBranch() == branch;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataMember ReferenceCache::findForTag(QSharedPointer<Tag> tag)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&tag](DataMember c) {
        return c->isTag() && c->toTag() == tag;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataMember ReferenceCache::findForRemote(QSharedPointer<Remote> remote)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&remote](DataMember c) {
        return c->isRemote() && c->toRemote() == remote;
    });

    if (i != mList.end())
        return *i;
    return {};
}

ReferenceCache::DataMember ReferenceCache::findForCommit(QSharedPointer<Commit> commit)
{
    if (!mList.size())
        fill();
    auto i = std::find_if(mList.begin(), mList.end(), [&commit](DataMember c) {
        return *c->target().data() == commit->commitHash();
    });

    if (i != mList.end())
        return *i;
    return {};
}

void ReferenceCache::fill()
{
    git_reference_iterator *iterator;
    git_reference *reference;
    BEGIN;
    STEP git_reference_iterator_new(&iterator, manager->repoPtr());

    if (IS_ERROR)
        return;

    while (!git_reference_next(&reference, iterator)) {
        auto p = findByPtr(reference);
        std::cout << "   " << p->name().toStdString();
    }

    git_reference_iterator_free(iterator);
}
}
