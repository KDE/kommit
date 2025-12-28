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
#include "index.h"

#include <QIODevice>
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

    SequenceRunner r;
    r.run(git_revparse_single, &commitObject, manager->repoPtr(), hash.toLatin1().constData());
    r.run(git_commit_lookup, &commit, manager->repoPtr(), git_object_id(commitObject));

    if (r.isSuccess())
        return Cache::findByPtr(commit);

    return Commit{};
}

QList<Commit> CommitsCache::allCommits(SortingMode sortMode)
{
    QList<Commit> list;

    if (!manager->isValid())
        return list;

    git_revwalk *walker;
    git_oid oid;

    SequenceRunner r;
    r.run(git_revwalk_new, &walker, manager->repoPtr());
    r.run(git_revwalk_sorting, walker, static_cast<unsigned int>(sortMode));
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

    if (r.isError())
        return list;

    while (!git_revwalk_next(&oid, walker)) {
        auto en = findByOid(&oid);
        en.clearChildren();
        list << en;
    }

    for (auto &commit : list) {
        for (auto const &parentHash : commit.parents()) {
            auto parent = find(parentHash);
            parent.addChild(commit.commitHash());
        }
        commit.setReferences(manager->references()->findForCommit(commit));
    }

    git_revwalk_free(walker);
    return list;
}

QList<Commit> CommitsCache::commitsInBranch(const Branch &branch)
{
    QList<Commit> list;

    git_revwalk *walker;
    git_oid oid;

    SequenceRunner r;
    r.run(git_revwalk_new, &walker, manager->repoPtr());
    r.run(git_revwalk_sorting, walker, GIT_SORT_TOPOLOGICAL);

    auto refName = git_reference_name(branch.refPtr());
    r.run(git_revwalk_push_ref, walker, refName);

    if (r.isError())
        return list;

    while (!git_revwalk_next(&oid, walker))
        list << findByOid(&oid);

    git_revwalk_free(walker);
    return list;
}

bool CommitsCache::create(const QString &message, const CommitOptions &opts)
{
    CommitOptions options{opts};
    options.setRepo(manager->repoPtr());

    SequenceRunner r;
    git_oid commit_oid;
    git_tree *tree = nullptr;
    git_commit *parent_commit = nullptr;

    auto indexPtr = manager->index();
    indexPtr.writeTree();
    auto lastTreeId = indexPtr.lastOid();

    r.run(git_tree_lookup, &tree, manager->repoPtr(), &lastTreeId);

    git_reference *head_ref = nullptr;
    git_oid parent_commit_oid;

    if (git_repository_head(&head_ref, manager->repoPtr()) == 0) {
        r.run(git_reference_name_to_id, &parent_commit_oid, manager->repoPtr(), "HEAD");
        r.run(git_commit_lookup, &parent_commit, manager->repoPtr(), &parent_commit_oid);
    }

    parent_commit = options.parentCommit();

    const git_commit *parents[1] = {const_cast<git_commit *>(parent_commit)};

    r.run(git_commit_create,
          &commit_oid,
          manager->repoPtr(),
          "HEAD",
          options.author().isNull() ? Signature(manager).data() : options.author().data(),
          options.committer().isNull() ? Signature(manager).data() : options.committer().data(),
          nullptr,
          message.toUtf8().constData(),
          tree,
          parent_commit ? 1 : 0,
          parent_commit ? parents : nullptr);

    r.printError();

    git_tree_free(tree);
    git_commit_free(parent_commit);
    git_reference_free(head_ref);

    // manager->d_ptr->index = Index{};

    auto newCommit = findByOid(&commit_oid);
    Q_EMIT added(newCommit);
    Q_EMIT reloadRequired();
    return r.isSuccess();
}

bool CommitsCache::amend(const QString &message, const CommitOptions &opts)
{
    CommitOptions options{opts};
    options.setRepo(manager->repoPtr());

    git_oid commit_oid;
    git_tree *tree = nullptr;
    SequenceRunner r;

    auto index = manager->index();
    index.writeTree();
    auto lastTreeId = index.lastOid();

    r.run(git_tree_lookup, &tree, manager->repoPtr(), &lastTreeId);

    r.run(git_commit_amend,
          &commit_oid,
          options.parentCommit(),
          "HEAD",
          options.author().isNull() ? Signature(manager).data() : options.author().data(),
          options.committer().isNull() ? Signature(manager).data() : options.committer().data(),
          nullptr,
          message.toUtf8().constData(),
          tree);

    git_tree_free(tree);

    return r.isSuccess();
}

void CommitsCache::clearChildData()
{
}

}

#include "moc_commitscache.cpp"
