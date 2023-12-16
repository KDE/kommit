/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stash.h"
#include "entities/commit.h"

#include "gitmanager.h"
#include <utility>

#include <git2/stash.h>
namespace Git
{
Stash::Stash(Manager *git, QString name)
    : mGit(git)
    , mMessage(std::move(name))
{
}

Stash::Stash(size_t index, git_repository *repo, const char *message, const git_oid *stash_id)
    : mIndex{index}
{
    git_commit *commit = NULL;

    git_commit_lookup(&commit, repo, stash_id);
    mCommit.reset(new Commit{commit});

    mSubject = git_commit_body(commit);

    mMessage = message;

    mSubject = QString{git_commit_message(commit)}.replace("\n", "");

    mCommitHash = git_oid_tostr_s(stash_id);
}

Stash::~Stash()
{
    if (ptr)
        git_commit_free(ptr);
}

const QString &Stash::message() const
{
    return mMessage;
}

const QString &Stash::subject() const
{
    return mSubject;
}

QSharedPointer<Commit> Stash::commit() const
{
    return mCommit;
}

QString Stash::commitHash() const
{
    return mCommitHash;
}

size_t Stash::index() const
{
    return mIndex;
}

} // namespace Git
