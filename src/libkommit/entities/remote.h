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

class Branch;
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

    [[nodiscard]] QString statusText() const;
};

class LIBKOMMIT_EXPORT RefSpec
{
public:
    enum class Direction { DirectionFetch = 0, DirectionPush = 1 };

    RefSpec(const git_refspec *refspecs);
    ~RefSpec();

    [[nodiscard]] QString name() const;
    [[nodiscard]] Direction direction() const;
    [[nodiscard]] QString destionation() const;
    [[nodiscard]] QString source() const;

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
    //    QList<RemoteBranch> branches;

    [[nodiscard]] QString name() const;
    [[nodiscard]] QList<RefSpec *> refSpecList() const;
    [[nodiscard]] QString pushUrl() const;
    [[nodiscard]] QString fetchUrl() const;
    [[nodiscard]] QString defaultBranch() const;

    [[nodiscard]] const QList<Branch *> &branches() const;

    [[nodiscard]] bool connected() const;

private:
    QList<RefSpec *> mRefSpecList;
    bool mConnected{false};
    QString mName;
    QString mPushUrl;
    QString mFetchUrl;
    QString mDefaultBranch;
    QList<Branch *> mBranches;

    friend class RemotesModel;
};

} // namespace Git
