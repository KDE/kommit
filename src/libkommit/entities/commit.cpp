/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commit.h"
#include "branch.h"
#include "note.h"
#include "oid.h"
#include "repository.h"

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
    ~CommitPrivate();

    git_commit *const commit;
    QString hash;
    Signature author;
    Signature committer;
    QList<Reference> references;
    Branch branch;
    Note note;
    Commit::CommitType type;
    QStringList parentHashes;
    QStringList children;
};

Commit::Commit() noexcept
    : d{new CommitPrivate{this, nullptr}}
{
}

Commit::Commit(git_commit *commit)
    : d{new CommitPrivate{this, commit}}
{
}

Commit::Commit(const Commit &other)
    : d{other.d}
{
}

Commit &Commit::operator=(const Commit &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Commit::operator==(const Commit &other) const
{
    return d->commit == other.d->commit;
}

bool Commit::operator!=(const Commit &other) const
{
    return !(*this == other);
}

bool Commit::operator<(const Commit &other) const
{
    return d->commit < other.d->commit;
}

bool Commit::isNull() const
{
    return !d->commit;
}

const Branch &Commit::branch() const
{
    return d->branch;
}

const QStringList &Commit::children() const
{
    return d->children;
}

const QList<Reference> &Commit::references() const
{
    return d->references;
}

Tree Commit::tree() const
{
    git_tree *tree;
    if (git_commit_tree(&tree, d->commit))
        return Tree{};
    return Tree{tree};
}

QString Commit::treeTitle() const
{
    return message();
}

git_commit *Commit::gitCommit() const
{
    return d->commit;
}

Note Commit::note() const
{
    if (d->note.isNull()) {
        git_note *note;

        if (!git_note_read(&note, git_commit_owner(d->commit), NULL, git_commit_id(d->commit)))
            d->note = Note{note};
    }
    return d->note;
}

QDateTime Commit::commitTime() const
{
    auto time = git_commit_time(d->commit);
    auto timeOffset = git_commit_time_offset(d->commit);

    return QDateTime::fromSecsSinceEpoch(time, QTimeZone{timeOffset});
}

git_commit *Commit::data() const
{
    return d->commit;
}

const git_commit *Commit::constData() const
{
    return d->commit;
}

const Signature &Commit::author() const
{
    if (d->author.isNull()) {
        auto sign = git_commit_author(d->commit);
        d->author = Signature{sign};
    }
    return d->author;
}

const Signature &Commit::committer() const
{
    if (d->committer.isNull()) {
        auto committer = git_commit_committer(d->commit);
        d->committer = Signature{committer};
    }
    return d->committer;
}

QString Commit::message() const
{
    return QString{git_commit_message(d->commit)}.remove(QLatin1Char('\n'));
}

QString Commit::body() const
{
    return QString{git_commit_body(d->commit)}.remove(QLatin1Char('\n'));
}

QString Commit::summary() const
{
    return QString{git_commit_summary(d->commit)}.remove(QLatin1Char('\n'));
}

const QString &Commit::commitHash() const
{
    return d->hash;
}

const QStringList &Commit::parents() const
{
    return d->parentHashes;
}

bool Commit::createNote(const QString &message)
{
    git_oid oid;
    auto repo = git_commit_owner(d->commit);
    auto commitOid = git_commit_id(d->commit);
    auto author = git_commit_author(d->commit);
    auto committer = git_commit_committer(d->commit);

    auto r = git_note_create(&oid, repo, NULL, author, committer, commitOid, message.toUtf8().data(), 1);
    if (r) {
        const git_error *lg2err;
        if ((lg2err = git_error_last()) != NULL && lg2err->message != NULL) {
            const QString msg = QString::fromLatin1(lg2err->message);
            qWarning() << "Error" << Q_FUNC_INFO << r << ":" << msg;
        }
    }
    return !r;
}

Oid Commit::oid() const
{
    return Oid{git_commit_id(d->commit)};
}

void Commit::clearChildren()
{
    d->children.clear();
}

void Commit::setReferences(const QList<Reference> refs)
{
    d->references = refs;
}

void Commit::addChild(const QString &childHash)
{
    d->children << childHash;
}

CommitPrivate::CommitPrivate(Commit *parent, git_commit *commit)
    : q_ptr{parent}
    , commit{commit}
{
    if (commit) {
        auto id = git_commit_id(commit);
        hash = git_oid_tostr_s(id);

        auto parentCount = git_commit_parentcount(commit);
        for (unsigned int i = 0; i < parentCount; ++i) {
            auto pid = git_commit_parent_id(commit, i);
            parentHashes << convertToString(pid, 20);
        }
    }
}

CommitPrivate::~CommitPrivate()
{
    git_commit_free(commit);
}
}
