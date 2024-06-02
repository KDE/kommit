/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commands/addsubmodulecommand.h"

namespace Git
{

AddSubmoduleCommand::AddSubmoduleCommand(Manager *git)
    : AbstractCommand{git}
{
}

QStringList AddSubmoduleCommand::generateArgs() const
{
    QStringList args{QStringLiteral("submodule"), QStringLiteral("add"), mUrl, mLocalPath};

    if (!mBranch.isEmpty())
        args << QStringLiteral("--branch=") + mBranch;

    if (mForce)
        args << QStringLiteral("--force");

    return args;
}

bool AddSubmoduleCommand::force() const
{
    return mForce;
}

void AddSubmoduleCommand::setForce(bool newForce)
{
    mForce = newForce;
}

QString AddSubmoduleCommand::branch() const
{
    return mBranch;
}

void AddSubmoduleCommand::setbranch(const QString &newbranch)
{
    mBranch = newbranch;
}

const QString &AddSubmoduleCommand::url() const
{
    return mUrl;
}

void AddSubmoduleCommand::setUrl(const QString &newUrl)
{
    mUrl = newUrl;
}

const QString &AddSubmoduleCommand::localPath() const
{
    return mLocalPath;
}

void AddSubmoduleCommand::setLocalPath(const QString &newLocalPath)
{
    mLocalPath = newLocalPath;
}

} // namespace Git

#include "moc_addsubmodulecommand.cpp"
