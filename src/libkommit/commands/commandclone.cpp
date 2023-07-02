/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandclone.h"

namespace Git
{

CloneCommand::CloneCommand(QObject *parent)
    : AbstractCommand(parent)
{
}

const QString &CloneCommand::repoUrl() const
{
    return mRepoUrl;
}

void CloneCommand::setRepoUrl(const QString &newRepoUrl)
{
    mRepoUrl = newRepoUrl;
}

const QString &CloneCommand::localPath() const
{
    return mLocalPath;
}

void CloneCommand::setLocalPath(const QString &newLocalPath)
{
    mLocalPath = newLocalPath;
}

const QString &CloneCommand::branch() const
{
    return mBranch;
}

void CloneCommand::setBranch(const QString &newBranch)
{
    mBranch = newBranch;
}

int CloneCommand::depth() const
{
    return mDepth;
}

void CloneCommand::setDepth(int newDepth)
{
    mDepth = newDepth;
}

const QString &CloneCommand::origin() const
{
    return mOrigin;
}

void CloneCommand::setOrigin(const QString &newOrigin)
{
    mOrigin = newOrigin;
}

bool CloneCommand::recursive() const
{
    return mRecursive;
}

void CloneCommand::setRecursive(bool newRecursive)
{
    mRecursive = newRecursive;
}

QStringList CloneCommand::generateArgs() const
{
    QStringList args{QStringLiteral("clone"), QStringLiteral("--progress"), mRepoUrl, mLocalPath};

    if (!mBranch.isEmpty())
        args << QStringLiteral("--branch=") + mBranch;

    if (mDepth != -1)
        args << QStringLiteral("--depth") << QString::number(mDepth);

    if (!mOrigin.isEmpty())
        args << QStringLiteral("--origin=") << mOrigin;

    if (mRecursive)
        args << QStringLiteral("--recursive");

    return args;
}

void CloneCommand::parseOutputSection(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(output)
    auto p = errorOutput.mid(0, errorOutput.lastIndexOf("%"));
    p = p.mid(p.lastIndexOf(" ") + 1);
    setProgress(p.toDouble());
}

bool CloneCommand::supportProgress() const
{
    return true;
}

} // namespace Git

#include "moc_commandclone.cpp"
