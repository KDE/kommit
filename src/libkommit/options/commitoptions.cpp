/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitoptions.h"
#include "commit.h"
#include "gitglobal_p.h"

#include <QTimeZone>

#include <git2/commit.h>
#include <git2/errors.h>
#include <git2/refs.h>
#include <git2/repository.h>
#include <git2/signature.h>

namespace Git
{

class CommitOptionsPrivate
{
public:
    git_repository *repo;

    Signature author;
    Signature committer;
    QString reflogMessage;

    Branch branch;
};

CommitOptions::CommitOptions()
    : d{new CommitOptionsPrivate}
{
}

QString CommitOptions::reflogMessage() const
{
    return d->reflogMessage;
}

void CommitOptions::setReflogMessage(const QString &reflogMessage)
{
    d->reflogMessage = reflogMessage;
}

Branch CommitOptions::branch() const
{
    return d->branch;
}

void CommitOptions::setBranch(const Branch &branch)
{
    d->branch = branch;
}

Signature CommitOptions::author() const
{
    return d->author;
}

git_commit *CommitOptions::parentCommit() const
{
    git_commit *parent_commit = nullptr;
    git_oid parent_commit_oid;

    if (d->branch.isNull()) {
        SequenceRunner r;
        git_reference *head_ref = nullptr;

        if (git_repository_head(&head_ref, d->repo) == 0) {
            r.run(git_reference_name_to_id, &parent_commit_oid, d->repo, "HEAD");
            r.run(git_commit_lookup, &parent_commit, d->repo, &parent_commit_oid);
        }
    } else {
        return d->branch.commit().data();
    }

    return parent_commit;
}

void CommitOptions::setRepo(git_repository *repo)
{
    d->repo = repo;
}

void CommitOptions::apply(git_commit_create_options *options)
{
    options->version = GIT_COMMIT_CREATE_OPTIONS_VERSION;
    if (!d->author.isNull())
        options->author = d->author.data();
    if (!d->committer.isNull())
        options->committer = d->committer.data();
}

void CommitOptions::setAuthor(const Signature &author)
{
    d->author = author;
}

Signature CommitOptions::committer() const
{
    return d->committer;
}

void CommitOptions::setCommitter(const Signature &committer)
{
    d->committer = committer;
}
}
