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
#include "types.h"

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

    if (!manager->isValid())
        return list;

    git_revwalk *walker;
    git_oid oid;

    BEGIN
    STEP git_revwalk_new(&walker, manager->repoPtr());
    STEP git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);
    // STEP git_revwalk_push_head(walker);

    // include all branches
    git_reference *ref;
    git_branch_iterator *it;
    git_branch_t b;

    git_branch_iterator_new(&it, manager->repoPtr(), GIT_BRANCH_ALL);

    while (!git_branch_next(&ref, &b, it)) {
        auto refname = git_reference_name(ref);
        git_revwalk_push_ref(walker, refname);
    }
    git_branch_iterator_free(it);

    if (IS_ERROR)
        return list;

    while (!git_revwalk_next(&oid, walker)) {
        auto en = findByOid(&oid);
        en->clearChildren();
        list << en;
    }

    for (auto &commit : list) {
        for (auto const &parentHash : commit->parents()) {
            auto parent = find(parentHash);
            parent->addChild(commit->commitHash());
        }
        commit->setReferences(manager->references()->findForCommit(commit));
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

void CommitsCache::clearChildData()
{
}

}

#include "moc_commitscache.cpp"
