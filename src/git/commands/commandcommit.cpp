/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandcommit.h"

namespace Git
{

CommandCommit::CommandCommit() = default;

QStringList CommandCommit::generateArgs() const
{
    QStringList args{QStringLiteral("commit"), QStringLiteral("--message"), mMessage};

    if (mAmend)
        args.append(QStringLiteral("--amend"));

    if (mIncludeStatus)
        args.append(QStringLiteral("--status"));
    else
        args.append(QStringLiteral("--no-status"));

    return args;
}

const QString &CommandCommit::message() const
{
    return mMessage;
}

void CommandCommit::setMessage(const QString &newMessage)
{
    mMessage = newMessage;
}

bool CommandCommit::amend() const
{
    return mAmend;
}

void CommandCommit::setAmend(bool newAmend)
{
    mAmend = newAmend;
}

bool CommandCommit::includeStatus() const
{
    return mIncludeStatus;
}

void CommandCommit::setIncludeStatus(bool newIncludeStatus)
{
    mIncludeStatus = newIncludeStatus;
}

} // namespace Git
