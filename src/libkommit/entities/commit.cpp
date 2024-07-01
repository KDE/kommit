/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commit.h"
#include "branch.h"
#include "gitmanager.h"
#include "note.h"
#include "oid.h"

#include <QTimeZone>

#include "tree.h"
#include "types.h"
#include <git2/commit.h>
#include <git2/errors.h>
#include <git2/notes.h>
#include <git2/revparse.h>
#include <git2/signature.h>

namespace Git
{

class CommitPrivate
{
    Commit *q_ptr;
    Q_DECLARE_PUBLIC(Commit)
public:
    CommitPrivate(Commit *parent, git_commit *commit);

    git_commit *const gitCommitPtr;
    QSharedPointer<Signature> author;
    QSharedPointer<Signature> committer;
    QSharedPointer<Reference> mReference;
    QSharedPointer<Branch> branch;
    QVector<GraphLane> mLanes;
    QSharedPointer<Note> note;
    Commit::CommitType type;
};

Commit::Commit(git_commit *commit)
    : d_ptr{new CommitPrivate{this, commit}}
{
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
    Q_D(Commit);
    git_commit_free(d->gitCommitPtr);
}

QSharedPointer<Branch> Commit::branch() const
{
    Q_D(const Commit);
    return d->branch;
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
    Q_D(const Commit);
    git_tree *tree;
    if (git_commit_tree(&tree, d->gitCommitPtr))
        return nullptr;
    return QSharedPointer<Tree>{new Tree{tree}};
}

QString Commit::treeTitle() const
{
    return message();
}

git_commit *Commit::gitCommit() const
{
    Q_D(const Commit);
    return d->gitCommitPtr;
}

QSharedPointer<Note> Commit::note()
{
    Q_D(Commit);

    if (d->note.isNull()) {
        git_note *note;

        if (!git_note_read(&note, git_commit_owner(d->gitCommitPtr), NULL, git_commit_id(d->gitCommitPtr)))
            d->note = QSharedPointer<Note>{new Note{note}};
    }
    return d->note;
}

QDateTime Commit::commitTime() const
{
    Q_D(const Commit);
    auto time = git_commit_time(d->gitCommitPtr);
    auto timeOffset = git_commit_time_offset(d->gitCommitPtr);

    return QDateTime::fromSecsSinceEpoch(time, QTimeZone{timeOffset});
}

QSharedPointer<Signature> Commit::author()
{
    Q_D(Commit);
    if (d->author.isNull()) {
        auto sign = git_commit_author(d->gitCommitPtr);
        d->author.reset(new Signature{sign});
    }
    return d->author;
}

QSharedPointer<Signature> Commit::committer()
{
    Q_D(Commit);
    if (d->committer.isNull()) {
        auto committer = git_commit_committer(d->gitCommitPtr);
        d->committer.reset(new Signature{committer});
    }
    return d->committer;
}

QString Commit::message() const
{
    Q_D(const Commit);
    return QString{git_commit_message(d->gitCommitPtr)}.remove(QLatin1Char('\n'));
}

QString Commit::subject() const
{
    Q_D(const Commit);
    return QString{git_commit_message(d->gitCommitPtr)}.remove(QLatin1Char('\n'));
}

QString Commit::body() const
{
    Q_D(const Commit);
    return QString{git_commit_body(d->gitCommitPtr)}.remove(QLatin1Char('\n'));
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
    Q_D(Commit);
    git_oid oid;
    auto repo = git_commit_owner(d->gitCommitPtr);
    auto commitOid = git_commit_id(d->gitCommitPtr);
    auto author = git_commit_author(d->gitCommitPtr);
    auto committer = git_commit_committer(d->gitCommitPtr);

    auto r = git_note_create(&oid, repo, NULL, author, committer, commitOid, message.toUtf8().data(), 1);
    if (r) {
        const git_error *lg2err;
        if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {
            const QString msg = QString::fromLatin1(lg2err->message);
            qDebug() << "Error" << Q_FUNC_INFO << r << ":" << msg;
        }
    }
    return !r;
}

QSharedPointer<Oid> Commit::oid() const
{
    Q_D(const Commit);
    return QSharedPointer<Oid>{new Oid{git_commit_id(d->gitCommitPtr)}};
}

CommitPrivate::CommitPrivate(Commit *parent, git_commit *commit)
    : q_ptr{parent}
    , gitCommitPtr{commit}
{
}
}
