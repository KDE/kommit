/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "gitgraphlane.h"
#include "libgitklient_export.h"
#include <QDateTime>
#include <QString>
#include <QVector>

namespace Git
{

class LIBGITKLIENT_EXPORT Log
{
public:
    enum CommitType { NormalCommit, InitialCommit, ForkCommit, MergeCommit };

    Log();
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

    const QString &authorName() const;
    const QString &authorEmail() const;
    const QDateTime &authDate() const;
    const QString &committerName() const;
    const QString &committerEmail() const;
    const QDateTime &commitDate() const;
    const QString &message() const;
    const QString &subject() const;
    const QString &body() const;
    const QString &commitHash() const;
    const QStringList &parents() const;
    const QString &refLog() const;
    const QString &branch() const;
    const QString &extraData() const;
    CommitType type() const;
    const QVector<GraphLane> &lanes() const;

    const QStringList &childs() const;

    const QString &commitShortHash() const;

private:
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
    friend class GitManager;
    friend class LogsModel;
};

}
