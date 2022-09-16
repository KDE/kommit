/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandpush.h"

namespace Git
{

CommandPush::CommandPush()
    : AbstractCommand()
{
}

const QString &CommandPush::remote() const
{
    return mRemote;
}

void CommandPush::setRemote(const QString &newRemote)
{
    mRemote = newRemote;
}

const QString &CommandPush::localBranch() const
{
    return mLocalBranch;
}

void CommandPush::setLocalBranch(const QString &newBranch)
{
    mLocalBranch = newBranch;
}

const QString &CommandPush::remoteBranch() const
{
    return mRemoteBranch;
}

void CommandPush::setRemoteBranch(const QString &newRemoteBranch)
{
    mRemoteBranch = newRemoteBranch;
}

bool CommandPush::force() const
{
    return mForce;
}

void CommandPush::setForce(bool newForce)
{
    mForce = newForce;
}

QStringList CommandPush::generateArgs() const
{
    QStringList args{QStringLiteral("push"), mRemote};

    if (mRemoteBranch.isEmpty())
        args.append(mLocalBranch);
    else
        args.append(mLocalBranch + QLatin1Char(':') + mRemoteBranch);

    if (mForce)
        args.append(QStringLiteral("--force"));

    return args;
}

} // namespace Git
