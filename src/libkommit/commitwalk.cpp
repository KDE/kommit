/*
SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitwalk.h"

#include <git2/branch.h>
#include <git2/refs.h>
#include <git2/repository.h>
#include <git2/revwalk.h>

namespace Git
{

CommitWalk walkCommits(const QString &path, const QString &branchRefName, int maxCount)
{
    CommitWalk result;

    if (path.isEmpty())
        return result;

    git_repository *repo{nullptr};
    if (git_repository_open_ext(&repo, path.toUtf8().constData(), 0, nullptr))
        return result;

    git_revwalk *walker{nullptr};
    if (git_revwalk_new(&walker, repo)) {
        git_repository_free(repo);
        return result;
    }

    if (branchRefName.isEmpty()) {
        git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL | GIT_SORT_TIME);

        git_branch_iterator *iterator{nullptr};
        if (!git_branch_iterator_new(&iterator, repo, GIT_BRANCH_ALL)) {
            git_reference *ref{nullptr};
            git_branch_t type;
            while (!git_branch_next(&ref, &type, iterator))
                git_revwalk_push_ref(walker, git_reference_name(ref));
            git_branch_iterator_free(iterator);
        }
    } else {
        git_revwalk_sorting(walker, GIT_SORT_TOPOLOGICAL);
        git_revwalk_push_ref(walker, branchRefName.toUtf8().constData());
    }

    git_oid oid;
    while (!git_revwalk_next(&oid, walker)) {
        result.oids << oid;

        if (maxCount > 0 && result.oids.size() >= maxCount) {
            // One step past the limit, so the caller is told whether anything was left
            // behind rather than having to guess from the count.
            result.hasMore = !git_revwalk_next(&oid, walker);
            break;
        }
    }

    git_revwalk_free(walker);
    git_repository_free(repo);

    return result;
}

}
