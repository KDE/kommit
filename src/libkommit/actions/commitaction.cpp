/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitaction.h"
#include "commit.h"
#include "gitglobal_p.h"
#include "repository.h"
#include "index.h"

#include <git2/repository.h>
#include <git2/signature.h>
#include <git2/refs.h>
#include <git2/version.h>

namespace Git {


class CommitActionPrivate
{
public:
    ~CommitActionPrivate();

    Repository *manager;
    git_repository *repo;
    git_signature *author{};
    git_signature *committer{};

    bool amend{false};
    bool allowEmptyCommit{false};
    QString message;
    QString authorName;
    QString authorEmail;
    QDateTime authTime;
    QString committerName;
    QString committerEmail;
    QDateTime commitTime;
    QString reflogMessage;

    Branch branch;

    bool createSignature(const QString &name, const QString &email, const QDateTime &time, git_signature **signature);
    git_commit *parentCommit();

    bool create();
    bool createAmend();
};


CommitActionPrivate::~CommitActionPrivate()
{
    if (author)
        git_signature_free(author);
    if (committer)
        git_signature_free(committer);
}

bool CommitActionPrivate::createSignature(const QString &name, const QString &email, const QDateTime &time, git_signature **signature)
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

git_commit *CommitActionPrivate::parentCommit()
{
    git_commit *parent_commit = nullptr;
    git_oid parent_commit_oid;

    if (branch.isNull()) {
        SequenceRunner r;
        git_reference *head_ref = nullptr;

        if (git_repository_head(&head_ref, repo) == 0) {
            r.run(git_reference_name_to_id, &parent_commit_oid, repo, "HEAD");
            r.run(git_commit_lookup, &parent_commit, repo, &parent_commit_oid);
        }
    } else {
        return branch.commit().data();
    }

    return parent_commit;
}

bool CommitActionPrivate::create()
{
#if LIBGIT2_VERSION_CHECK(1, 8, 0)
    createSignature(authorName, authorEmail, authTime, &author);
    createSignature(committerName, committerEmail, commitTime, &committer);

    git_commit_create_options opts = GIT_COMMIT_CREATE_OPTIONS_INIT;
    git_oid id;
    opts.author = author;
    opts.committer = committer;
    opts.allow_empty_commit = allowEmptyCommit;
    return git_commit_create_from_stage(&id, repo, message.toUtf8().constData(), &opts);
#else
    return false;
#endif
}

bool CommitActionPrivate::createAmend()
{
    createSignature(authorName, authorEmail, authTime, &author);
    createSignature(committerName, committerEmail, commitTime, &committer);

    git_oid commit_oid;
    git_tree *tree = nullptr;
    SequenceRunner r;

    auto index = manager->index();
    index.writeTree();
    auto lastTreeId = index.lastOid();

    r.run(git_tree_lookup, &tree, repo, &lastTreeId);

    r.run(git_commit_amend,
          &commit_oid,
          parentCommit(),
          "HEAD",
          author,
          committer,
          nullptr,
          message.toUtf8().constData(),
          tree);

    git_tree_free(tree);

    return 0;
}

CommitAction::CommitAction(Repository *repo)
    : d{new CommitActionPrivate}
{
    d->manager = repo;
    d->repo = repo->repoPtr();
}

bool CommitAction::amend() const
{
    return d->amend;
}

void CommitAction::setAmend(const bool &amend)
{
    d->amend = amend;
}

bool CommitAction::allowEmptyCommit() const
{
    return d->allowEmptyCommit;
}

void CommitAction::setAllowEmptyCommit(const bool &allowEmptyCommit)
{
    d->allowEmptyCommit = allowEmptyCommit;
}

QString CommitAction::message() const
{
    return d->message;
}

void CommitAction::setMessage(const QString &message)
{
    d->message = message;
}

QString CommitAction::authorName() const
{
    return d->authorName;
}

void CommitAction::setAuthorName(const QString &authorName)
{
    d->authorName = authorName;
}

QString CommitAction::authorEmail() const
{
    return d->authorEmail;
}

void CommitAction::setAuthorEmail(const QString &authorEmail)
{
    d->authorEmail = authorEmail;
}

QDateTime CommitAction::authTime() const
{
    return d->authTime;
}

void CommitAction::setAuthTime(const QDateTime &authTime)
{
    d->authTime = authTime;
}

QString CommitAction::committerName() const
{
    return d->committerName;
}

void CommitAction::setCommitterName(const QString &committerName)
{
    d->committerName = committerName;
}

QString CommitAction::committerEmail() const
{
    return d->committerEmail;
}

void CommitAction::setCommitterEmail(const QString &committerEmail)
{
    d->committerEmail = committerEmail;
}

QDateTime CommitAction::commitTime() const
{
    return d->commitTime;
}

void CommitAction::setCommitTime(const QDateTime &commitTime)
{
    d->commitTime = commitTime;
}

int CommitAction::exec()
{
    return 0;
}

}
