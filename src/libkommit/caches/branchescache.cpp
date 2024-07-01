/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchescache.h"
#include "entities/branch.h"
#include "gitglobal_p.h"

#include <git2/branch.h>

namespace Git
{

BranchesCache::BranchesCache(git_repository *repo)
    : Git::AbstractCache<Branch>{repo}
{
}

Branch *BranchesCache::lookup(const QString &key, git_repository *repo)
{
    git_reference *ref;
    BEGIN
    STEP git_branch_lookup(&ref, repo, key.toLocal8Bit().data(), GIT_BRANCH_ALL);

    if (IS_OK)
        return new Branch{ref};
    return nullptr;
}

};
