/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "gitgraphlane.h"
#include "interfaces.h"
#include "libkommit_export.h"
#include "reference.h"
#include "signature.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>
#include <QVector>

#include <git2/types.h>

namespace Git
{

class Tree;
class Note;
class CommitPrivate;

class LIBKOMMIT_EXPORT Commit : public ITree, public IOid
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    explicit Commit(git_commit *commit);
    ~Commit();

    Q_REQUIRED_RESULT QSharedPointer<Signature> author();
    Q_REQUIRED_RESULT QSharedPointer<Signature> committer();
    Q_REQUIRED_RESULT QString message() const;
    Q_DECL_DEPRECATED_X("Use message")
    Q_REQUIRED_RESULT QString subject() const;
    Q_REQUIRED_RESULT QString body() const;
    Q_REQUIRED_RESULT const QString &commitHash() const;
    Q_REQUIRED_RESULT const QStringList &parents() const;
    Q_REQUIRED_RESULT QSharedPointer<Branch> branch() const;
    Q_REQUIRED_RESULT const QString &extraData() const;
    Q_REQUIRED_RESULT CommitType type() const;
    Q_REQUIRED_RESULT const QVector<GraphLane> &lanes() const;
    Q_REQUIRED_RESULT const QStringList &children() const;
    Q_REQUIRED_RESULT const QString &commitShortHash() const;
    Q_REQUIRED_RESULT QDateTime commitTime() const;

    Q_REQUIRED_RESULT QSharedPointer<Reference> reference() const;

    Q_REQUIRED_RESULT QSharedPointer<Tree> tree() const override;
    Q_REQUIRED_RESULT QString treeTitle() const override;

    Q_REQUIRED_RESULT git_commit *gitCommit() const;

    Q_REQUIRED_RESULT QSharedPointer<Note> note();
    bool createNote(const QString &message);
    QSharedPointer<Oid> oid() const override;

private:
    CommitPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Commit);

    // QString mMessage;
    // QString mSubject;
    // QString mBody;
    QString mCommitHash;
    QString mCommitShortHash;
    QStringList mParentHash;
    QString mExtraData;
    CommitType mType;
    QVector<GraphLane> mLanes;
    QStringList mChildren;
    QSharedPointer<Reference> mReference;

    friend class LogList;
    friend class Manager;
    friend class LogsModel;
};
}
