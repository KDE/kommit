/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"
#include <QList>
#include <QString>

#include <git2/remote.h>

namespace Git
{

class LIBKOMMIT_EXPORT RemoteBranch
{
public:
    bool configuredPull{false};
    bool configuredPush{false};

    QString name;
    QString remotePushBranch;
    QString remotePullBranch;
    enum class Status { Unknown, UpToDate, FastForwardable, LocalOutOfDate };
    Status status{Status::Unknown};

    Q_REQUIRED_RESULT QString statusText() const;
};

class LIBKOMMIT_EXPORT RefSpec
{
public:
    enum class Direction { DirectionFetch = 0, DirectionPush = 1 };

    RefSpec(const git_refspec *refspecs);

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT Direction direction() const;
    Q_REQUIRED_RESULT QString destionation() const;
    Q_REQUIRED_RESULT QString source() const;

private:
    QString mName;
    Direction mDirection;
    QString mDestionation;
    QString mSource;
};

class LIBKOMMIT_EXPORT Remote
{
public:
    Remote();
    Remote(git_remote *remote);

    void parse(const QString &output);
    //    QString headBranch;
    QList<RemoteBranch> branches;

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QList<RefSpec *> refSpecList() const;
    Q_REQUIRED_RESULT QString pushUrl() const;
    Q_REQUIRED_RESULT QString fetchUrl() const;
    Q_REQUIRED_RESULT QString defaultBranch() const;

private:
    QList<RefSpec *> mRefSpecList;
    QString mName;
    QString mPushUrl;
    QString mFetchUrl;
    QString mDefaultBranch;
};

} // namespace Git
