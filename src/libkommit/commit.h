/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "gitgraphlane.h"
#include "libkommit_export.h"
#include <QDateTime>
#include <QString>
#include <QVector>
#include <git2/types.h>

namespace Git
{

class LIBKOMMIT_EXPORT Log
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    Log();
    Log(git_commit *commit);
    Log(QString authorName,
        QString authorEmail,
        QDateTime authDate,
        QString committerName,
        QString committerEmail,
        QDateTime commitDate,
        QString message,
        QString subject,
        QString body,
        QString commitHash,
        QStringList parentHash);
    ~Log();

    Q_REQUIRED_RESULT const QString &authorName() const;
    Q_REQUIRED_RESULT const QString &authorEmail() const;
    Q_REQUIRED_RESULT const QDateTime &authDate() const;
    Q_REQUIRED_RESULT const QString &committerName() const;
    Q_REQUIRED_RESULT const QString &committerEmail() const;
    Q_REQUIRED_RESULT const QDateTime &commitDate() const;
    Q_REQUIRED_RESULT const QString &message() const;
    Q_REQUIRED_RESULT const QString &subject() const;
    Q_REQUIRED_RESULT const QString &body() const;
    Q_REQUIRED_RESULT const QString &commitHash() const;
    Q_REQUIRED_RESULT const QStringList &parents() const;
    Q_REQUIRED_RESULT const QString &refLog() const;
    Q_REQUIRED_RESULT const QString &branch() const;
    Q_REQUIRED_RESULT const QString &extraData() const;
    Q_REQUIRED_RESULT CommitType type() const;
    Q_REQUIRED_RESULT const QVector<GraphLane> &lanes() const;

    Q_REQUIRED_RESULT const QStringList &childs() const;

    Q_REQUIRED_RESULT const QString &commitShortHash() const;

private:
    git_commit *mGitCommit{nullptr};
    QString mAuthorName;
    QString mAuthorEmail;
    QDateTime mAuthDate;
    QString mCommitterName;
    QString mCommitterEmail;
    QDateTime mCommitDate;
    QString mMessage;
    QString mSubject;
    QString mBody;
    QString mCommitHash;
    QString mCommitShortHash;
    QStringList mParentHash;
    QString mRefLog;
    QString mBranch;
    QString mExtraData;
    CommitType mType;
    QVector<GraphLane> mLanes;
    QStringList mChilds;

    friend class LogList;
    friend class Manager;
    friend class LogsModel;
};

}
