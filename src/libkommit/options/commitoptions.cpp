/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitoptions.h"
#include "gitglobal_p.h"
#include "commit.h"

#include <QTimeZone>

#include <git2/signature.h>
#include <git2/errors.h>
#include <git2/repository.h>
#include <git2/refs.h>
#include <git2/commit.h>

namespace Git
{

class CommitOptionsPrivate
{
public:
    ~CommitOptionsPrivate();

    git_repository *repo;
    git_signature *author{};
    git_signature *committer{};

    Signature auth;

    QString authorName;
    QString authorEmail;
    QDateTime authTime;
    QString committerName;
    QString committerEmail;
    QDateTime commitTime;
    QString reflogMessage;

    Branch branch;

    bool createSignature(const QString &name, const QString &email, const QDateTime &time, git_signature **signature);
};

CommitOptions::CommitOptions()
{
    d.reset(new CommitOptionsPrivate);
}

CommitOptions::CommitOptions(const CommitOptions &other)
{
    d = other.d;
}

QString CommitOptions::authorName() const
{
    return d->authorName;
}

void CommitOptions::setAuthorName(const QString &authorName)
{
    d->authorName = authorName;
}

QString CommitOptions::authorEmail() const
{
    return d->authorEmail;
}

void CommitOptions::setAuthorEmail(const QString &authorEmail)
{
    d->authorEmail = authorEmail;
}

QDateTime CommitOptions::authTime() const
{
    return d->authTime;
}

void CommitOptions::setAuthTime(const QDateTime &authTime)
{
    d->authTime = authTime;
}

QString CommitOptions::committerName() const
{
    return d->committerName;
}

void CommitOptions::setCommitterName(const QString &committerName)
{
    d->committerName = committerName;
}

QString CommitOptions::committerEmail() const
{
    return d->committerEmail;
}

void CommitOptions::setCommitterEmail(const QString &committerEmail)
{
    d->committerEmail = committerEmail;
}

QDateTime CommitOptions::commitTime() const
{
    return d->commitTime;
}

void CommitOptions::setCommitTime(const QDateTime &commitTime)
{
    d->commitTime = commitTime;
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

git_signature *CommitOptions::author() const
{
    d->createSignature(d->authorName, d->authorEmail, d->authTime, &d->author);
    return d->author;
}

git_signature *CommitOptions::committer() const
{
    d->createSignature(d->committerName, d->committerEmail, d->commitTime, &d->committer);
    return d->committer;
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

CommitOptionsPrivate::~CommitOptionsPrivate()
{
    if (author)
        git_signature_free(author);
    if (committer)
        git_signature_free(committer);
}

bool CommitOptionsPrivate::createSignature(const QString &name, const QString &email, const QDateTime &time, git_signature **signature)
{
    if (*signature) {
        // If the signature pointer is already valid, return early
        return true;
    }

    int error = 0;

    if (name.isEmpty() || email.isEmpty()) {
        // If name or email is empty, create a default signature for the repository
        error = git_signature_default(signature, repo);
        if (error != 0) {
            // const git_error *err = giterr_last();
            // qWarning() << "Failed to create default git signature:" << err->message;
            return false;
        }
        return true;
    }

    if (!time.isValid()) {
        // If the time is not valid, use the current time
        error = git_signature_now(signature, name.toUtf8().constData(), email.toUtf8().constData());
    } else {
        // Convert QDateTime to git_time
        git_time gitTime;
        gitTime.time = time.toSecsSinceEpoch(); // Convert QDateTime to Unix timestamp
        gitTime.offset = time.offsetFromUtc() / 60; // Convert time zone offset to minutes

        // Create the git signature using libgit2
        error = git_signature_new(signature, name.toUtf8().constData(), email.toUtf8().constData(), gitTime.time, gitTime.offset);
    }

    return !error;
}

}
