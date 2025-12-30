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
#include "repository.h"
#include "types.h"

#include <git2/annotated_commit.h>
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
    ~BranchPrivate();

    git_reference *const branch;
    QString name;
    QString refName;
    QString upStreamName;
    QString remoteName;

    // Commit commit;
};

Branch::Branch()
    : d{new BranchPrivate{this, nullptr}}
{
}

Branch::Branch(git_reference *branch)
    : d{new BranchPrivate{this, branch}}
{
    if (branch) {
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
}

Branch::Branch(const Branch &other)
    : d{other.d}
{
}

Branch &Branch::operator=(const Branch &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Branch::operator==(const Branch &other) const
{
    return d->branch == other.d->branch;
}

bool Branch::operator!=(const Branch &other) const
{
    return !(*this == other);
}

bool Branch::operator<(const Branch &other) const
{
    return d->branch < other.d->branch;
}

bool Branch::isNull() const
{
    return !d->branch;
}

git_reference *Branch::data() const
{
    return d->branch;
}

const git_reference *Branch::constData() const
{
    return d->branch;
}

QString Branch::name() const
{
    return d->name;
}

QString Branch::refName() const
{
    return d->refName;
}

QString Branch::upStreamName() const
{
    return d->upStreamName;
}

QString Branch::remoteName() const
{
    return d->remoteName;
}

bool Branch::isHead() const
{
    return git_branch_is_head(d->branch) == 1;
}

Object Branch::object() const
{
    git_object *object;

    if (git_reference_peel(&object, d->branch, GIT_OBJECT_COMMIT))
        return Object{};
    return Object{object};
}

Tree Branch::tree() const
{
    git_commit *commit;
    git_object *obj;
    git_tree *tree;

    auto repo = git_reference_owner(d->branch);

    SequenceRunner r;

    r.run(git_revparse_single, &obj, repo, toConstChars(d->refName));
    r.run(git_commit_lookup, &commit, repo, git_object_id(obj));
    r.run(git_commit_tree, &tree, commit);

    if (r.isError())
        return Tree{};

    return Tree{tree};
}

Commit Branch::commit()
{
    // if (d->commit.isNull()) {
    git_commit *commit;
    git_object *obj;

    auto repo = git_reference_owner(d->branch);
    BEGIN
    STEP git_revparse_single(&obj, repo, toConstChars(d->refName));
    STEP git_commit_lookup(&commit, repo, git_object_id(obj));
    END;

    if (IS_OK)
        return Repository::owner(repo)->commits()->findByPtr(commit);
    // }

    return Commit{};
}

AnnotatedCommit Branch::annotatedCommit()
{
    git_annotated_commit *c;
    auto repo = git_reference_owner(d->branch);

    auto ok = SequenceRunner::runSingle(git_annotated_commit_from_ref, &c, repo, d->branch);

    if (ok)
        return AnnotatedCommit{c};
    return AnnotatedCommit{nullptr};
}

Reference Branch::reference() const
{
    auto manager = Repository::owner(git_reference_owner(d->branch));
    return manager->references()->findByPtr(d->branch);
}

QString Branch::treeTitle() const
{
    return name();
}

git_reference *Branch::refPtr() const
{
    return d->branch;
}

BranchPrivate::BranchPrivate(Branch *parent, git_reference *branch)
    : q_ptr{parent}
    , branch{branch}
{
}

BranchPrivate::~BranchPrivate()
{
    git_reference_free(branch);
}
}
