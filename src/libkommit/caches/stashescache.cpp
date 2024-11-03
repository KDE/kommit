/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashescache.h"
#include "caches/commitscache.h"
#include "gitglobal_p.h"
#include "repository.h"
#include "types.h"

#include <git2/errors.h>
#include <git2/stash.h>

#include <QDebug>

namespace Git
{

class StashesCachePrivate
{
    StashesCache *q_ptr;
    Q_DECLARE_PUBLIC(StashesCache)

public:
    StashesCachePrivate(StashesCache *parent, Repository *manager);
    Repository *manager;
    StashesCache::ListType list;
};

StashesCache::StashesCache(Repository *manager)
    : QObject{manager}
    , d_ptr{new StashesCachePrivate{this, manager}}
{
}

StashesCache::~StashesCache()
{
}

StashesCache::DataType StashesCache::findByName(const QString &name)
{
    auto list = allStashes();

    auto i = std::find_if(list.begin(), list.end(), [&name](StashesCache::DataType stash) {
        return stash.message() == name;
    });

    if (i == list.end())
        return Stash{};
    return *i;
}

bool StashesCache::create(const QString &name)
{
    Q_D(StashesCache);
    git_oid oid;
    git_signature *sign;

    BEGIN
    STEP git_signature_default(&sign, d->manager->repoPtr());
    STEP git_stash_save(&oid, d->manager->repoPtr(), sign, name.toUtf8().data(), GIT_STASH_DEFAULT);

    return IS_OK;
}

bool StashesCache::apply(const Stash &stash)
{
    if (stash.isNull())
        return false;

    Q_D(StashesCache);

    git_stash_apply_options options;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_apply(d->manager->repoPtr(), stash.index(), &options);

    PRINT_ERROR;

    return IS_OK;
}

bool StashesCache::pop(const Stash &stash)
{
    if (stash.isNull())
        return false;

    Q_D(StashesCache);

    git_stash_apply_options options;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_pop(d->manager->repoPtr(), stash.index(), &options);

    if (IS_OK)
        Q_EMIT reloadRequired();

    return IS_OK;
}

bool StashesCache::remove(const Stash &stash)
{
    if (stash.isNull())
        return false;

    Q_D(StashesCache);

    BEGIN
    STEP git_stash_drop(d->manager->repoPtr(), stash.index());

    if (IS_OK)
        Q_EMIT reloadRequired();

    return IS_OK;
}

bool StashesCache::apply(const QString &name)
{
    Q_D(const StashesCache);

    auto stashIndex = findIndex(name);
    git_stash_apply_options options;

    if (stashIndex == -1)
        return false;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_apply(d->manager->repoPtr(), stashIndex, &options);

    // TODO: update commits model
    PRINT_ERROR;

    return IS_OK;
}

bool StashesCache::remove(const QString &name)
{
    Q_D(const StashesCache);

    auto stashIndex = findIndex(name);

    if (stashIndex == -1)
        return false;

    BEGIN
    STEP git_stash_drop(d->manager->repoPtr(), stashIndex);

    if (IS_OK)
        Q_EMIT reloadRequired();

    return IS_OK;
}

bool StashesCache::pop(const QString &name)
{
    Q_D(const StashesCache);

    auto stashIndex = findIndex(name);
    git_stash_apply_options options;

    if (stashIndex == -1)
        return false;

    BEGIN
    STEP git_stash_apply_options_init(&options, GIT_STASH_APPLY_OPTIONS_VERSION);
    STEP git_stash_pop(d->manager->repoPtr(), stashIndex, &options);

    if (IS_OK)
        Q_EMIT reloadRequired();

    return IS_OK;
}
StashesCache::ListType StashesCache::allStashes()
{
    Q_D(StashesCache);

    struct wrapper {
        git_repository *repo;
        QList<Stash> list;
        Repository *manager;
    };

    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        auto w = static_cast<wrapper *>(payload);

        w->list << Stash{w->manager, index, message, stash_id};

        return 0;
    };

    wrapper w;
    w.repo = d->manager->repoPtr();
    w.manager = d->manager;
    git_stash_foreach(w.repo, callback, &w);
    return w.list;
}

int StashesCache::findIndex(const QString &message) const
{
    Q_D(const StashesCache);

    struct wrapper {
        int index{-1};
        QString name;
    };
    wrapper w;
    w.name = message;
    auto callback = [](size_t index, const char *message, const git_oid *stash_id, void *payload) {
        Q_UNUSED(stash_id)
        auto w = reinterpret_cast<wrapper *>(payload);
        if (message == w->name)
            w->index = index;
        return 0;
    };
    git_stash_foreach(d->manager->repoPtr(), callback, &w);

    return w.index;
}
void StashesCache::clear()
{
}

StashesCachePrivate::StashesCachePrivate(StashesCache *parent, Repository *manager)
    : q_ptr{parent}
    , manager{manager}
{
}
}

#include "moc_stashescache.cpp"
