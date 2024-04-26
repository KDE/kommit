/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commit.h"
#include "note.h"

#include <QTimeZone>

#include "tree.h"
#include "types.h"
#include <git2/commit.h>
#include <git2/errors.h>
#include <git2/notes.h>
#include <git2/revparse.h>
#include <git2/signature.h>
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

    auto time = git_commit_time(commit);
    auto timeOffset = git_commit_time_offset(commit);

    mCommitTime = QDateTime::fromSecsSinceEpoch(time, QTimeZone{timeOffset});
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

const QStringList &Commit::children() const
{
    return mChildren;
}

const QString &Commit::commitShortHash() const
{
    return mCommitShortHash;
}

QSharedPointer<Reference> Commit::reference() const
{
    return mReference;
}

QSharedPointer<Tree> Commit::tree() const
{
    git_tree *tree;
    if (git_commit_tree(&tree, mGitCommit))
        return nullptr;
    return QSharedPointer<Tree>{new Tree{tree}};
}

QString Commit::treeTitle() const
{
    return mSubject;
}

git_commit *Commit::gitCommit() const
{
    return mGitCommit;
}

QSharedPointer<Note> Commit::note() const
{
    git_note *note;
    if (git_note_read(&note, git_commit_owner(mGitCommit), NULL, git_commit_id(mGitCommit)))
        return {};
    return QSharedPointer<Note>{new Note{note}};
}

QDateTime Commit::commitTime() const
{
    return mCommitTime;
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
bool Commit::createNote(const QString &message)
{
    git_oid oid;
    auto repo = git_commit_owner(mGitCommit);
    auto commitOid = git_commit_id(mGitCommit);
    auto author = git_commit_author(mGitCommit);
    auto committer = git_commit_committer(mGitCommit);

    auto r = git_note_create(&oid, repo, NULL, author, committer, commitOid, message.toUtf8().data(), 1);
    if (r) {
        const git_error *lg2err;
        if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {
            auto msg = QString{lg2err->message};
            qDebug() << "Error" << Q_FUNC_INFO << r << ":" << msg;
        }
    }
    return !r;
}
}
