/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotescache.h"
#include "entities/remote.h"
#include "gitglobal_p.h"

#include <git2/remote.h>

namespace Git
{

RemotesCache::RemotesCache(git_repository *repo)
    : Git::AbstractCache<Remote>{repo}
{
}

Remote *RemotesCache::lookup(const QString &key, git_repository *repo)
{
    git_remote *remote;
    BEGIN
    STEP git_remote_lookup(&remote, repo, key.toLocal8Bit().data());
    if (IS_OK)
        return new Remote{remote};
    return nullptr;
}
};
