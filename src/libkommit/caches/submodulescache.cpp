/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submodulescache.h"

#include <QDebug>
#include <git2/errors.h>
#include <git2/submodule.h>

#include "gitglobal_p.h"
#include "options/addsubmoduleoptions.h"
#include "repository.h"
#include "types.h"

namespace Git
{
SubmodulesCache::SubmodulesCache(Repository *manager)
    : QObject{manager}
    , Cache<Submodule, git_submodule>{manager}
{
}

SubmodulesCache::DataType SubmodulesCache::add(const AddSubmoduleOptions &options)
{
    git_submodule *submodule{nullptr};
    git_repository *submoduleRepo;
    git_submodule_update_options opts = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

    options.applyToFetchOptions(&opts.fetch_opts);
    options.applyToCheckoutOptions(&opts.checkout_opts);

    BEGIN
    STEP git_submodule_add_setup(&submodule, manager->repoPtr(), toConstChars(options.url), toConstChars(options.path), 1);
    STEP git_submodule_clone(&submoduleRepo, submodule, &opts);
    STEP git_submodule_add_finalize(submodule);
    PRINT_ERROR;

    if (IS_OK) {
        auto en = findByPtr(submodule);

        if (!en.isNull()) {
            Q_EMIT added(en);
            return en;
        }
    }

    return DataType{};
}

QList<QSharedPointer<Submodule>> SubmodulesCache::allSubmodules()
{
    struct Data {
        PointerList<Submodule> list;
        git_repository *repo;
        SubmodulesCache *cache;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(name);

        auto data = reinterpret_cast<Data *>(payload);

        // Wait until libgit-1.2 and use lines below
        //  git_submodule *submodule;
        //  git_submodule_dup(&submodule, sm);

        data->list.append(data->cache->findByPtr(sm));

        return 0;
    };

    Data data;
    data.repo = manager->repoPtr();
    data.cache = this;
    git_submodule_foreach(data.repo, cb, &data);

    return data.list;
}

QSharedPointer<Submodule> SubmodulesCache::findByName(const QString &name)
{
    git_submodule *submodule{nullptr};
    BEGIN
    STEP git_submodule_lookup(&submodule, manager->repoPtr(), toConstChars(name));
    PRINT_ERROR;

    if (IS_ERROR)
        return nullptr;

    return findByPtr(submodule);
}

SubmodulesCache::DataType SubmodulesCache::findByPtr(git_submodule *ptr, bool *isNew)
{
    if (mHash.contains(ptr)) {
        if (isNew)
            *isNew = false;
        return mHash.value(ptr);
    }

    QSharedPointer<Submodule> entity{new Submodule{ptr, manager->repoPtr()}};
    mList << entity;
    mHash.insert(ptr, entity);

    if (isNew)
        *isNew = true;
    return entity;
}

void SubmodulesCache::clearChildData()
{
}
}

#include "moc_submodulescache.cpp"
