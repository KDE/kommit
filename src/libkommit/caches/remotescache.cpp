/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotescache.h"
#include "entities/remote.h"
#include "gitglobal_p.h"
#include "gitmanager.h"

#include <git2/remote.h>
#include <git2/strarray.h>

#include <QDebug>

#include "types.h"

namespace Git
{

RemotesCache::RemotesCache(Manager *manager)
    : Cache<Remote, git_remote>{manager}
{
}

QList<QSharedPointer<Remote>> RemotesCache::allRemotes()
{
    QList<QSharedPointer<Remote>> list;
    git_strarray names{};
    git_remote_list(&names, manager->repoPtr());
    auto remotes = convert(&names);
    git_strarray_free(&names);

    for (auto &r : remotes) {
        git_remote *remote;
        if (!git_remote_lookup(&remote, manager->repoPtr(), r.toLatin1().data())) {
            auto rem = findByPtr(remote);
            if (!rem.isNull())
                list << rem;
        }
    }
    return list;
}

QSharedPointer<Remote> RemotesCache::findByName(const QString &name)
{
    git_remote *remote;
    BEGIN
    STEP git_remote_lookup(&remote, manager->repoPtr(), name.toLocal8Bit().data());

    if (IS_OK)
        return Cache::findByPtr(remote);

    return QSharedPointer<Remote>{};
}

bool RemotesCache::create(const QString &name, const QString &url)
{
    git_remote *remote;
    BEGIN
    STEP git_remote_create(&remote, manager->repoPtr(), name.toUtf8().data(), url.toUtf8().data());
    END;

    if (IS_OK)
        Cache::findByPtr(remote);

    return IS_OK;
}

bool RemotesCache::setUrl(DataMember remote, const QString &url)
{
    return false;
}

bool RemotesCache::remove(QSharedPointer<Remote> remote)
{
    BEGIN
    STEP git_remote_delete(manager->repoPtr(), remote->name().toUtf8().data());
    END;

    return IS_OK;
}

bool RemotesCache::rename(QSharedPointer<Remote> remote, const QString &newName)
{
    git_strarray problems = {0};

    BEGIN
    STEP git_remote_rename(&problems, manager->repoPtr(), remote->name().toUtf8().data(), newName.toUtf8().data());
    git_strarray_free(&problems);

    return IS_OK;
}
};
