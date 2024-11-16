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

SubmodulesCache::DataType SubmodulesCache::add(AddSubmoduleOptions *options)
{
    git_submodule *submodule{nullptr};
    git_repository *submoduleRepo;
    git_submodule_update_options opts = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

    options->fetchOptions()->apply(&opts.fetch_opts);
    options->checkoutOptions()->applyToCheckoutOptions(&opts.checkout_opts);

    SequenceRunner r;
    r.run(git_submodule_add_setup, &submodule, manager->repoPtr(), toConstChars(options->url()), toConstChars(options->path()), 1);
    r.run(git_submodule_clone, &submoduleRepo, submodule, &opts);
    r.run(git_submodule_add_finalize, submodule);

    if (r.isSuccess()) {
        auto en = findByPtr(submodule);

        if (!en.isNull()) {
            Q_EMIT added(en);
            return en;
        }
    }

    return DataType{};
}

QList<Submodule> SubmodulesCache::allSubmodules()
{
    struct Data {
        QList<Submodule> list;
        git_repository *repo;
        SubmodulesCache *cache;
    };

    auto cb = [](git_submodule *sm, const char *name, void *payload) -> int {
        Q_UNUSED(name);

        auto data = reinterpret_cast<Data *>(payload);

        git_submodule *submodule;
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 2, 0)
        git_submodule_dup(&submodule, sm);
#else
        git_submodule_lookup(&submodule, data->repo, name);
#endif
        data->list.append(data->cache->findByPtr(submodule));

        return 0;
    };

    Data data;
    data.repo = manager->repoPtr();
    data.cache = this;
    git_submodule_foreach(data.repo, cb, &data);

    return data.list;
}

Submodule SubmodulesCache::findByName(const QString &name)
{
    git_submodule *submodule{nullptr};

    if (!SequenceRunner::runSingle(git_submodule_lookup, &submodule, manager->repoPtr(), toConstChars(name)))
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

    Submodule entity{ptr, manager->repoPtr()};
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
