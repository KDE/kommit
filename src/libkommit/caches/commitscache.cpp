/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitscache.h"
#include "caches/referencecache.h"
#include "entities/branch.h"
#include "entities/commit.h"
#include "gitglobal_p.h"
#include "gitmanager.h"

#include <git2/commit.h>
#include <git2/revparse.h>

namespace Git
{

CommitsCache::CommitsCache(Manager *parent)
    : Git::OidCache<Commit, git_commit>{parent, git_commit_lookup}
{
}

QSharedPointer<Commit> CommitsCache::find(const QString &hash)
{
    git_commit *commit;
    git_object *commitObject;
    BEGIN
    STEP git_revparse_single(&commitObject, manager->repoPtr(), hash.toLatin1().constData());
    STEP git_commit_lookup(&commit, manager->repoPtr(), git_object_id(commitObject));

    if (IS_OK)
        return Cache::findByPtr(commit);

    return QSharedPointer<Commit>{};
}

QList<QSharedPointer<Commit>> CommitsCache::allCommits()
{
    PointerList<Commit> list;

    git_revwalk *walker;
    git_oid oid;

    BEGIN
    STEP git_revwalk_new(&walker, manager->repoPtr());
    STEP git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
    STEP git_revwalk_push_head(walker);

    if (IS_ERROR)
        return list;

    while (!git_revwalk_next(&oid, walker))
        list << findByOid(&oid);

    for (auto &commit : list) {
        for (auto const &parentHash : commit->parents()) {
            auto parent = find(parentHash);
            parent->mChildren << commit->commitHash();
        }
        commit->mReference = manager->referencesCache()->findForCommit(commit);
    }

    git_revwalk_free(walker);
    return list;
}

QList<QSharedPointer<Commit>> CommitsCache::commitsInBranch(QSharedPointer<Branch> branch)
{
    PointerList<Commit> list;

    git_revwalk *walker;
    git_oid oid;

    BEGIN
    STEP git_revwalk_new(&walker, manager->repoPtr());
    STEP git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);

    auto refName = git_reference_name(branch->refPtr());
    STEP git_revwalk_push_ref(walker, refName);

    if (IS_ERROR)
        return list;

    while (!git_revwalk_next(&oid, walker))
        list << findByOid(&oid);

    git_revwalk_free(walker);
    return list;
}
};

#include "moc_commitscache.cpp"
