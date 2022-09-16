/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stash.h"

#include "gitmanager.h"
#include <utility>

namespace Git
{

const QString &Stash::name() const
{
    return mName;
}

const QString &Stash::authorName() const
{
    return mAuthorName;
}

const QString &Stash::authorEmail() const
{
    return mAuthorEmail;
}

const QString &Stash::subject() const
{
    return mSubject;
}

const QString &Stash::branch() const
{
    return mBranch;
}

const QDateTime &Stash::pushTime() const
{
    return mPushTime;
}

Stash::Stash(Manager *git, QString name)
    : mGit(git)
    , mName(std::move(name))
{
}

void Stash::apply()
{
    mGit->runGit({"stash", "apply", mName});
}

void Stash::drop()
{
    mGit->runGit({"stash", "drop", mName});
}

void Stash::pop()
{
    mGit->runGit({"stash", "pop", mName});
}

} // namespace Git
