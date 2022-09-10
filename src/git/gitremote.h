/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QList>
#include <QString>


namespace Git {

class RemoteBranch
{
public:
    bool configuredPull{false};
    bool configuredPush{false};

    QString name;
    QString remotePushBranch;
    QString remotePullBranch;
    enum class Status
    {
        Unknown,
        UpToDate,
        FastForwardable,
        LocalOutOfDate
    };
    Status status{Status::Unknown};

    QString statusText() const;
};

class Remote
{
public:
    QString name;
    QString headBranch;
    QString fetchUrl;
    QString pushUrl;
    QList<RemoteBranch> branches;
    //    QStringList
    Remote();
    void parse(const QString &output);
};

} // namespace Git

