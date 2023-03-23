/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitlog.h"

#include <utility>

namespace Git
{

const QString &Log::refLog() const
{
    return mRefLog;
}

const QString &Log::branch() const
{
    return mBranch;
}

const QString &Log::extraData() const
{
    return mExtraData;
}

Log::CommitType Log::type() const
{
    return mType;
}

const QVector<GraphLane> &Log::lanes() const
{
    return mLanes;
}

const QStringList &Log::childs() const
{
    return mChilds;
}

const QString &Log::commitShortHash() const
{
    return mCommitShortHash;
}

Log::Log() = default;

Log::Log(QString authorName,
         QString authorEmail,
         QDateTime authDate,
         QString committerName,
         QString committerEmail,
         QDateTime commitDate,
         QString message,
         QString subject,
         QString body,
         QString commitHash,
         QStringList parentHash)
    : mAuthorName(std::move(authorName))
    , mAuthorEmail(std::move(authorEmail))
    , mAuthDate(std::move(authDate))
    , mCommitterName(std::move(committerName))
    , mCommitterEmail(std::move(committerEmail))
    , mCommitDate(std::move(commitDate))
    , mMessage(std::move(message))
    , mSubject(std::move(subject))
    , mBody(std::move(body))
    , mCommitHash(std::move(commitHash))
    , mParentHash(std::move(parentHash))
{
}

Log::~Log() = default;

const QString &Log::authorName() const
{
    return mAuthorName;
}

const QString &Log::authorEmail() const
{
    return mAuthorEmail;
}

const QDateTime &Log::authDate() const
{
    return mAuthDate;
}

const QString &Log::committerName() const
{
    return mCommitterName;
}

const QString &Log::committerEmail() const
{
    return mCommitterEmail;
}

const QDateTime &Log::commitDate() const
{
    return mCommitDate;
}

const QString &Log::message() const
{
    return mMessage;
}

const QString &Log::subject() const
{
    return mSubject;
}

const QString &Log::body() const
{
    return mBody;
}

const QString &Log::commitHash() const
{
    return mCommitHash;
}

const QStringList &Log::parents() const
{
    return mParentHash;
}

}
