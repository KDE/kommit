/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitscache.h"
#include "caches/referencecache.h"
#include "entities/branch.h"
#include "gitglobal_p.h"
#include "repository.h"
#include "types.h"

#include <git2/commit.h>
#include <git2/revparse.h>

namespace Git
{

CommitsCache::CommitsCache(Repository *parent)
    : Git::OidCache<Commit, git_commit>{parent, git_commit_lookup}
{
}

Commit CommitsCache::find(const QString &hash)
{
    git_commit *commit;
    git_object *commitObject;
    BEGIN
    STEP git_revparse_single(&commitObject, manager->repoPtr(), hash.toLatin1().constData());
    STEP git_commit_lookup(&commit, manager->repoPtr(), git_object_id(commitObject));

    if (IS_OK)
        return Cache::findByPtr(commit);

    return Commit{};
}

QList<Commit> CommitsCache::allCommits()
{
    QList<Commit> list;

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
        en.clearChildren();
        list << en;
    }

    linkCommits(list);

    git_revwalk_free(walker);
    return list;
}

QList<Commit> CommitsCache::commitsFromOids(const QList<git_oid> &oids)
{
    QList<Commit> list;
    list.reserve(oids.size());

    for (const auto &oid : oids) {
        auto en = findByOid(&oid);
        en.clearChildren();
        list << en;
    }

    linkCommits(list);

    return list;
}

void CommitsCache::linkCommits(QList<Commit> &list)
{
    for (auto &commit : list) {
        // An oid the repository does not hold gives back nothing, which has no parents to
        // read and no children to be given.
        if (commit.isNull())
            continue;

        // Look the parents up by oid rather than by their hash string: find() turns the
        // hash back into an oid with git_revparse_single(), once per parent, when the oid
        // is right there in the commit.
        auto commitPtr = commit.data();
        const auto parentCount = git_commit_parentcount(commitPtr);
        for (unsigned int i = 0; i < parentCount; ++i) {
            auto parent = findByOid(git_commit_parent_id(commitPtr, i));
            if (!parent.isNull())
                parent.addChild(commit.commitHash());
        }
        commit.setReferences(manager->references()->findForCommit(commit));
    }
}

QList<Commit> CommitsCache::commitsInBranch(const Branch &branch)
{
    QList<Commit> list;

    git_revwalk *walker;
    git_oid oid;

    BEGIN
    STEP git_revwalk_new(&walker, manager->repoPtr());
    STEP git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);

    auto refName = git_reference_name(branch.refPtr());
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
