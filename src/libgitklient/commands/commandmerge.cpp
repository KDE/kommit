/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandmerge.h"

#include <QMetaEnum>

namespace Git
{

CommandMerge::CommandMerge(Manager *git)
    : AbstractCommand{git}
{
}

QStringList CommandMerge::generateArgs() const
{
    QStringList cmd{QStringLiteral("merge")};
    switch (mFf) {
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

    switch (mStrategy) {
    case Ort:
        cmd << QStringLiteral("--strategy=ort");
        if (mIgnoreSpaceChange)
            cmd << QStringLiteral("--ignore-space-change");
        if (mIgnoreAllSpace)
            cmd << QStringLiteral("--ignore-all-space");
        if (mIgnoreSpaceAtEol)
            cmd << QStringLiteral("--ignore-space-at-eol");
        if (mIgnoreCrAtEol)
            cmd << QStringLiteral("--ignore-cr-at-eol");
        if (mRenormalize)
            cmd << QStringLiteral("--renormalize");
        if (mOurs)
            cmd << QStringLiteral("--ours");
        if (mTheirs)
            cmd << QStringLiteral("--theirs");
        break;
    case Recursive: {
        auto e = QMetaEnum::fromType<DiffAlgorithm>();
        cmd << QStringLiteral("diff-algorithm=") + QString(e.valueToKey(mDiffAlgorithm)).toLower();
        if (mNoRenames)
            cmd << QStringLiteral("--no-renames");
        cmd << QStringLiteral("--strategy=recursive");
        break;
    }
    case Resolve:
        cmd << QStringLiteral("--strategy=resolve");
        break;
    case Octopus:
        cmd << QStringLiteral("--strategy=octopus");
        break;
    case Ours:
        cmd << QStringLiteral("--strategy=ours");
        break;
    case Subtree:
        cmd << QStringLiteral("--strategy=subtree");
        break;
    case Default:
        // Just to avoid compiler warning
        break;
    }

    cmd << mFromBranch;

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
    return mFf;
}

void CommandMerge::setFf(FastForwardType newFf)
{
    mFf = newFf;
}

OptionalBool CommandMerge::squash() const
{
    return mSquash;
}

void CommandMerge::setSquash(OptionalBool newSquash)
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

const CommandMerge::Strategy &CommandMerge::strategy() const
{
    return mStrategy;
}

void CommandMerge::setStrategy(const Strategy &newStrategy)
{
    mStrategy = newStrategy;
}

bool CommandMerge::ignoreSpaceChange() const
{
    return mIgnoreSpaceChange;
}

void CommandMerge::setIgnoreSpaceChange(bool newIgnoreSpaceChange)
{
    mIgnoreSpaceChange = newIgnoreSpaceChange;
}

bool CommandMerge::ignoreAllSpace() const
{
    return mIgnoreAllSpace;
}

void CommandMerge::setIgnoreAllSpace(bool newIgnoreAllSpace)
{
    mIgnoreAllSpace = newIgnoreAllSpace;
}

bool CommandMerge::ignoreSpaceAtEol() const
{
    return mIgnoreSpaceAtEol;
}

void CommandMerge::setIgnoreSpaceAtEol(bool newIgnoreSpaceAtEol)
{
    mIgnoreSpaceAtEol = newIgnoreSpaceAtEol;
}

bool CommandMerge::ignoreCrAtEol() const
{
    return mIgnoreCrAtEol;
}

void CommandMerge::setIgnoreCrAtEol(bool newIgnoreCrAtEol)
{
    mIgnoreCrAtEol = newIgnoreCrAtEol;
}

bool CommandMerge::renormalize() const
{
    return mRenormalize;
}

void CommandMerge::setRenormalize(bool newRenormalize)
{
    mRenormalize = newRenormalize;
}

bool CommandMerge::noRenames() const
{
    return mNoRenames;
}

void CommandMerge::setNoRenames(bool newNoRenames)
{
    mNoRenames = newNoRenames;
}

CommandMerge::DiffAlgorithm CommandMerge::diffAlgorithm() const
{
    return mDiffAlgorithm;
}

void CommandMerge::setDiffAlgorithm(DiffAlgorithm newDiffAlgorithm)
{
    mDiffAlgorithm = newDiffAlgorithm;
}

bool CommandMerge::ours() const
{
    return mOurs;
}

void CommandMerge::setOurs(bool newOurs)
{
    mOurs = newOurs;
}

bool CommandMerge::theirs() const
{
    return mTheirs;
}

void CommandMerge::setTheirs(bool newTheirs)
{
    mTheirs = newTheirs;
}

} // namespace Git
