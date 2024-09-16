/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branch.h"
#include "buffer.h"
#include "caches/commitscache.h"
#include "caches/referencecache.h"
#include "entities/commit.h"
#include "entities/tree.h"
#include "gitglobal_p.h"
#include "gitmanager.h"
#include "types.h"

#include <git2/branch.h>
#include <git2/buffer.h>
#include <git2/commit.h>
#include <git2/errors.h>
#include <git2/graph.h>
#include <git2/notes.h>
#include <git2/object.h>
#include <git2/refs.h>
#include <git2/revparse.h>

namespace Git
{

class BranchPrivate
{
    Branch *q_ptr;
    Q_DECLARE_PUBLIC(Branch)

public:
    BranchPrivate(Branch *parent, git_reference *branch);

    git_reference *const branch;
    QString name;
    QString refName;
    QString upStreamName;
    QString remoteName;

    QSharedPointer<Commit> commit;
};

Branch::Branch(git_reference *branch)
    : d_ptr{new BranchPrivate{this, branch}}
{
    Q_D(Branch);

    const char *tmp;
    git_branch_name(&tmp, branch);
    d->name = tmp;
    auto refName = git_reference_name(branch);
    d->refName = refName;

    auto repo = git_reference_owner(branch);
    Buf buf;
    if (!git_branch_upstream_name(&buf, repo, refName))
        d->upStreamName = buf.toString();

    Buf buf2;
    if (!git_branch_remote_name(&buf2, repo, refName))
        d->remoteName = buf2.toString();
}

Branch::~Branch()
{
    Q_D(Branch);
    git_reference_free(d->branch);
    delete d;
}

QString Branch::name() const
{
    Q_D(const Branch);
    return d->name;
}

QString Branch::refName() const
{
    Q_D(const Branch);
    return d->refName;
}

QString Branch::upStreamName() const
{
    Q_D(const Branch);
    return d->upStreamName;
}

QString Branch::remoteName() const
{
    Q_D(const Branch);
    return d->remoteName;
}

bool Branch::isHead() const
{
    Q_D(const Branch);
    return git_branch_is_head(d->branch) == 1;
}

QSharedPointer<Object> Branch::object() const
{
    Q_D(const Branch);

    git_object *object;

    if (git_reference_peel(&object, d->branch, GIT_OBJECT_COMMIT))
        return QSharedPointer<Object>{};
    return QSharedPointer<Object>{new Object{object}};
}

QSharedPointer<Tree> Branch::tree() const
{
    Q_D(const Branch);
    git_commit *commit;
    git_object *obj;
    git_tree *tree;

    auto repo = git_reference_owner(d->branch);

    BEGIN
    STEP git_revparse_single(&obj, repo, toConstChars(d->refName));
    STEP git_commit_lookup(&commit, repo, git_object_id(obj));
    STEP git_commit_tree(&tree, commit);
    END;

    RETURN_COND(QSharedPointer<Tree>{new Tree{tree}}, nullptr);
}

QSharedPointer<Commit> Branch::commit()
{
    Q_D(Branch);

    if (d->commit.isNull()) {
        git_commit *commit;
        git_object *obj;

        auto repo = git_reference_owner(d->branch);
        BEGIN
        STEP git_revparse_single(&obj, repo, toConstChars(d->refName));
        STEP git_commit_lookup(&commit, repo, git_object_id(obj));
        END;

        if (IS_OK)
            d->commit = Manager::owner(repo)->commits()->findByPtr(commit);
    }

    return d->commit;
}

QSharedPointer<Reference> Branch::reference() const
{
    Q_D(const Branch);
    auto manager = Manager::owner(git_reference_owner(d->branch));
    return manager->references()->findByPtr(d->branch);
}

QString Branch::treeTitle() const
{
    return name();
}

git_reference *Branch::refPtr() const
{
    Q_D(const Branch);
    return d->branch;
}

BranchPrivate::BranchPrivate(Branch *parent, git_reference *branch)
    : q_ptr{parent}
    , branch{branch}
{
}
}
