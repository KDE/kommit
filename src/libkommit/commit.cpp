/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commit.h"

#include "types.h"
#include <git2/commit.h>
#include <git2/revparse.h>
#include <utility>

namespace Git
{

Commit::Commit() = default;

Commit::Commit(git_commit *commit)
    : mGitCommit{commit}
{
    mSubject = QString{git_commit_message(commit)}.replace("\n", "");

    auto committer = git_commit_committer(commit);
    mCommitter.reset(new Signature{committer});

    auto author = git_commit_author(commit);
    mAuthor.reset(new Signature{author});

    mBody = QString{git_commit_body(commit)}.replace("\n", "");

    auto id = git_commit_id(commit);
    mCommitHash = git_oid_tostr_s(id);

    auto parentCount = git_commit_parentcount(commit);
    for (unsigned int i = 0; i < parentCount; ++i) {
        auto pid = git_commit_parent_id(commit, i);
        mParentHash << convertToString(pid, 20);
    }
}

Commit::~Commit()
{
    git_commit_free(mGitCommit);
}

const QString &Commit::branch() const
{
    return mBranch;
}

const QString &Commit::extraData() const
{
    return mExtraData;
}

Commit::CommitType Commit::type() const
{
    return mType;
}

const QVector<GraphLane> &Commit::lanes() const
{
    return mLanes;
}

const QStringList &Commit::childs() const
{
    return mChilds;
}

const QString &Commit::commitShortHash() const
{
    return mCommitShortHash;
}

QSharedPointer<Reference> Commit::reference() const
{
    return mReference;
}

QSharedPointer<Signature> Commit::author() const
{
    return mAuthor;
}

QSharedPointer<Signature> Commit::committer() const
{
    return mCommitter;
}

const QString &Commit::message() const
{
    return mMessage;
}

const QString &Commit::subject() const
{
    return mSubject;
}

const QString &Commit::body() const
{
    return mBody;
}

const QString &Commit::commitHash() const
{
    return mCommitHash;
}

const QStringList &Commit::parents() const
{
    return mParentHash;
}

}
