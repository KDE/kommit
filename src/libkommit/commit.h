/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "gitgraphlane.h"
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

class LIBKOMMIT_EXPORT Commit
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    Commit();
    Commit(git_commit *commit);
    ~Commit();

    Q_REQUIRED_RESULT const Signature &author() const;
    Q_REQUIRED_RESULT const Signature &committer() const;
    Q_REQUIRED_RESULT const QString &message() const;
    Q_REQUIRED_RESULT const QString &subject() const;
    Q_REQUIRED_RESULT const QString &body() const;
    Q_REQUIRED_RESULT const QString &commitHash() const;
    Q_REQUIRED_RESULT const QStringList &parents() const;
    Q_REQUIRED_RESULT const QString &branch() const;
    Q_REQUIRED_RESULT const QString &extraData() const;
    Q_REQUIRED_RESULT CommitType type() const;
    Q_REQUIRED_RESULT const QVector<GraphLane> &lanes() const;
    Q_REQUIRED_RESULT const QStringList &childs() const;
    Q_REQUIRED_RESULT const QString &commitShortHash() const;

    Q_REQUIRED_RESULT QSharedPointer<Reference> reference() const;

private:
    git_commit *mGitCommit{nullptr};
    Signature mAuthor;
    Signature mCommitter;
    QString mMessage;
    QString mSubject;
    QString mBody;
    QString mCommitHash;
    QString mCommitShortHash;
    QStringList mParentHash;
    QString mBranch;
    QString mExtraData;
    CommitType mType;
    QVector<GraphLane> mLanes;
    QStringList mChilds;
    QSharedPointer<Reference> mReference;

    friend class LogList;
    friend class Manager;
    friend class LogsModel;
};

}
