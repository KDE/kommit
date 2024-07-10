/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandfetch.h"

namespace Git
{

bool CommandFetch::noFf() const
{
    return mNoFf;
}

void CommandFetch::setNoFf(bool newNoFf)
{
    mNoFf = newNoFf;
}

bool CommandFetch::ffOnly() const
{
    return mFfOnly;
}

void CommandFetch::setFfOnly(bool newFfOnly)
{
    mFfOnly = newFfOnly;
}

bool CommandFetch::noCommit() const
{
    return mNoCommit;
}

void CommandFetch::setNoCommit(bool newNoCommit)
{
    mNoCommit = newNoCommit;
}

bool CommandFetch::prune() const
{
    return mPrune;
}

void CommandFetch::setPrune(bool newPrune)
{
    mPrune = newPrune;
}

bool CommandFetch::tags() const
{
    return mTags;
}

void CommandFetch::setTags(bool newTags)
{
    mTags = newTags;
}

void CommandFetch::parseOutputSection(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(output)
    Q_UNUSED(errorOutput)
}

const QString &CommandFetch::remote() const
{
    return mRemote;
}

void CommandFetch::setRemote(const QString &newRemote)
{
    mRemote = newRemote;
}

const QString &CommandFetch::branch() const
{
    return mBranch;
}

void CommandFetch::setBranch(const QString &newBranch)
{
    mBranch = newBranch;
}

QStringList CommandFetch::generateArgs() const
{
    QStringList args{QStringLiteral("fetch"), mRemote};
    if (!mBranch.isEmpty())
        args.append(mBranch);
    // if (mNoFf)
    //     args.append(QStringLiteral("--no-ff"));
    // if (mFfOnly)
    //     args.append(QStringLiteral("--ff-only"));
    // if (mNoCommit)
    //     args.append(QStringLiteral("--no-commit"));
    if (mPrune)
        args.append(QStringLiteral("--prune"));
    if (mTags)
        args.append(QStringLiteral("--tags"));
    return args;
}

CommandFetch::CommandFetch() = default;

} // namespace Git
