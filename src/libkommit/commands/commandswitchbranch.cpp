/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandswitchbranch.h"

#include "libkommit_debug.h"

#include <KLocalizedString>

namespace Git
{

CommandSwitchBranch::CommandSwitchBranch(Manager *git)
    : AbstractCommand(git)
{
}

void CommandSwitchBranch::parseOutputSection(const QByteArray &output, const QByteArray &errorOutput)
{
    if (errorOutput.contains("error: Your local changes to the following files would be overwritten by checkout")) {
        setStatus(Error);
        setErrorMessage(
            i18n("Your local changes to the following files would be overwritten by checkout. "
                 "Please commit your changes or stash them before you switch branches."));
    }
    qCDebug(KOMMITLIB_LOG) << "Error" << errorOutput;
    qCDebug(KOMMITLIB_LOG) << "out" << output;
}

const QString &CommandSwitchBranch::target() const
{
    return mTarget;
}

void CommandSwitchBranch::setTarget(const QString &newTarget)
{
    mTarget = newTarget;
}

CommandSwitchBranch::Mode CommandSwitchBranch::mode() const
{
    return mMode;
}

void CommandSwitchBranch::setMode(Mode newMode)
{
    mMode = newMode;
}

QStringList CommandSwitchBranch::generateArgs() const
{
    QStringList cmd;

    if (mMode == NewBranch)
        cmd << QStringLiteral("checkout") << QStringLiteral("-b") << mTarget;
    else if (mMode == RemoteBranch)
        cmd << QStringLiteral("checkout") << QStringLiteral("-b") << mTarget << mRemoteBranch;
    else
        cmd << QStringLiteral("switch") << mTarget;

    if (mForce)
        cmd.append(QStringLiteral("--force"));
    return cmd;
}

bool CommandSwitchBranch::force() const
{
    return mForce;
}

void CommandSwitchBranch::setForce(bool newForce)
{
    mForce = newForce;
}

QString CommandSwitchBranch::remoteBranch() const
{
    return mRemoteBranch;
}

void CommandSwitchBranch::setRemoteBranch(const QString &newRemoteBranch)
{
    mRemoteBranch = newRemoteBranch;
}

} // namespace Git

#include "moc_commandswitchbranch.cpp"
