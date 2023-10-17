/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branch.h"
#include "entities/tree.h"
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
Branch::Branch(git_reference *branch)
    : mBranch{branch}
{
    const char *tmp;
    git_branch_name(&tmp, branch);
    mName = tmp;
    auto refName = git_reference_name(branch);
    mRefName = refName;

    auto repo = git_reference_owner(branch);

    auto buf = git_buf{0};
    if (!git_branch_upstream_name(&buf, repo, refName))
        mUpStreamName = buf.ptr;
    git_buf_dispose(&buf);

    auto buf2 = git_buf{0};
    if (!git_branch_remote_name(&buf2, repo, refName))
        mRemoteName = buf2.ptr;
    git_buf_dispose(&buf2);

    mIsHead = git_branch_is_head(branch);
}

Branch::~Branch()
{
    git_reference_free(mBranch);
}

QString Branch::name() const
{
    return mName;
}

QString Branch::refName() const
{
    return mRefName;
}

QString Branch::upStreamName() const
{
    return mUpStreamName;
}

QString Branch::remoteName() const
{
    return mRemoteName;
}

Note *Branch::note() const
{
    return nullptr;
}

bool Branch::isHead() const
{
    return mIsHead;
}

Tree *Branch::tree() const
{
    git_commit *commit;
    git_object *obj;
    git_tree *tree;

    auto repo = git_reference_owner(mBranch);

    BEGIN
    STEP git_revparse_single(&obj, repo, toConstChars(mRefName));
    STEP git_commit_lookup(&commit, repo, git_object_id(obj));
    STEP git_commit_tree(&tree, commit);
    END;

    RETURN_COND(new Tree{tree}, nullptr);
}
}
