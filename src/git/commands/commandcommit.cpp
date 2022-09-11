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
    QStringList args{QStringLiteral("commit"), QStringLiteral("--message"), _message};

    if (_amend)
        args.append(QStringLiteral("--amend"));

    if (_includeStatus)
        args.append(QStringLiteral("--status"));
    else
        args.append(QStringLiteral("--no-status"));

    return args;
}

const QString &CommandCommit::message() const
{
    return _message;
}

void CommandCommit::setMessage(const QString &newMessage)
{
    _message = newMessage;
}

bool CommandCommit::amend() const
{
    return _amend;
}

void CommandCommit::setAmend(bool newAmend)
{
    _amend = newAmend;
}

bool CommandCommit::includeStatus() const
{
    return _includeStatus;
}

void CommandCommit::setIncludeStatus(bool newIncludeStatus)
{
    _includeStatus = newIncludeStatus;
}

} // namespace Git
