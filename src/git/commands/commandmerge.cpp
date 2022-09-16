/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandmerge.h"

namespace Git
{

CommandMerge::CommandMerge(Manager *git)
    : AbstractCommand{git}
{
}

QStringList CommandMerge::generateArgs() const
{
    QStringList cmd{QStringLiteral("merge")};
    switch (_ff) {
    case FastForwardType::Unset:
        break;
    case FastForwardType::Yes:
        cmd.append(QStringLiteral("--ff"));
        break;
    case FastForwardType::No:
        cmd.append(QStringLiteral("--no-ff"));
        break;
    case FastForwardType::OnlyFastForward:
        cmd.append(QStringLiteral("--ff-only"));
        break;
    }

    appendBool(mCommit, cmd, QStringLiteral("commit"));
    appendBool(mSquash, cmd, QStringLiteral("squash"));
    appendBool(mAllowUnrelatedHistories, cmd, QStringLiteral("allow-unrelated-histories"));

    if (!mStrategy.isEmpty())
        cmd.append(QStringLiteral("--strategy=") + mStrategy);

    return cmd;
}

OptionalBool CommandMerge::commit() const
{
    return mCommit;
}

void CommandMerge::setCommit(OptionalBool newCommit)
{
    mCommit = newCommit;
}

OptionalBool CommandMerge::allowUnrelatedHistories() const
{
    return mAllowUnrelatedHistories;
}

void CommandMerge::setAllowUnrelatedHistories(OptionalBool newAllowUnrelatedHistories)
{
    mAllowUnrelatedHistories = newAllowUnrelatedHistories;
}

FastForwardType CommandMerge::ff() const
{
    return _ff;
}

void CommandMerge::setFf(FastForwardType newFf)
{
    _ff = newFf;
}

bool CommandMerge::squash() const
{
    return mSquash;
}

void CommandMerge::setSquash(bool newSquash)
{
    mSquash = newSquash;
}

const QString &CommandMerge::fromBranch() const
{
    return mFromBranch;
}

void CommandMerge::setFromBranch(const QString &newFromBranch)
{
    mFromBranch = newFromBranch;
}

const QString &CommandMerge::strategy() const
{
    return mStrategy;
}

void CommandMerge::setStrategy(const QString &newStrategy)
{
    mStrategy = newStrategy;
}

} // namespace Git
