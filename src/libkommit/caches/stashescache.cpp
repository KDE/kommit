/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashescache.h"
#include "entities/stash.h"
#include "gitglobal_p.h"
#include "gitmanager.h"

#include <git2/errors.h>
#include <git2/stash.h>

#include <QDebug>

namespace Git
{

StashesCache::StashesCache(Manager *manager)
    : manager{manager}
{
}

bool StashesCache::create(const QString &name)
{
    git_oid oid;
    git_signature *sign;

    BEGIN
    STEP git_signature_default(&sign, manager->repoPtr());
    STEP git_stash_save(&oid, manager->repoPtr(), sign, name.toUtf8().data(), GIT_STASH_DEFAULT);
    return IS_OK;
}

bool StashesCache::apply(QSharedPointer<Stash> stash)
{
    if (stash.isNull())
        return false;

    git_stash_apply_options options;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_apply(manager->repoPtr(), stash->index(), &options);

    PRINT_ERROR;

    return IS_OK;
}

bool StashesCache::pop(QSharedPointer<Stash> stash)
{
    if (stash.isNull())
        return false;

    git_stash_apply_options options;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_pop(manager->repoPtr(), stash->index(), &options);

    return IS_OK;
}

bool StashesCache::remove(QSharedPointer<Stash> stash)
{
    if (stash.isNull())
        return false;

    BEGIN
    STEP git_stash_drop(manager->repoPtr(), stash->index());

    return IS_OK;
}

bool StashesCache::drop(QSharedPointer<Stash> stash)
{
    return false;
}

QList<QSharedPointer<Stash>> StashesCache::allStashes()
{
    struct wrapper {
        git_repository *repo;
        PointerList<Stash> list;
    };

    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        auto w = static_cast<wrapper *>(payload);

        QSharedPointer<Stash> ptr{new Stash{index, w->repo, message, stash_id}};

        w->list << ptr;

        return 0;
    };

    wrapper w;
    w.repo = manager->repoPtr();
    git_stash_foreach(w.repo, callback, &w);
    return w.list;
}

void StashesCache::clear()
{
}
}
