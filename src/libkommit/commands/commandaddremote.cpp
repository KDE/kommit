/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandaddremote.h"

namespace Git
{

CommandAddRemote::CommandAddRemote(QObject *parent)
    : AbstractCommand{parent}
{
}

QStringList CommandAddRemote::generateArgs() const
{
    QStringList args{QStringLiteral("remote"), QStringLiteral("add")};

    if (mFetch)
        args << QStringLiteral("--fetch");
    if (mMirror)
        args << QStringLiteral("--mirror");
    appendBool(mTags, args, QStringLiteral("tags"));

    if (!mMaster.isEmpty())
        args << QStringLiteral("--master=") + mMaster;

    args << mRemoteName << mUrl;
    return args;
}

const QString &CommandAddRemote::remoteName() const
{
    return mRemoteName;
}

void CommandAddRemote::setRemoteName(const QString &newRemoteName)
{
    mRemoteName = newRemoteName;
}

OptionalBool CommandAddRemote::tags() const
{
    return mTags;
}

void CommandAddRemote::setTags(OptionalBool newTags)
{
    mTags = newTags;
}

void CommandAddRemote::setTags(Qt::CheckState newTags)
{
    setTags(checkStateToOptionalBool(newTags));
}

bool CommandAddRemote::mirror() const
{
    return mMirror;
}

void CommandAddRemote::setMirror(bool newMirror)
{
    mMirror = newMirror;
}

const QString &CommandAddRemote::master() const
{
    return mMaster;
}

void CommandAddRemote::setMaster(const QString &newMaster)
{
    mMaster = newMaster;
}

bool CommandAddRemote::fetch() const
{
    return mFetch;
}

void CommandAddRemote::setFetch(bool newFetch)
{
    mFetch = newFetch;
}

const QString &CommandAddRemote::url() const
{
    return mUrl;
}

void CommandAddRemote::setUrl(const QString &newUrl)
{
    mUrl = newUrl;
}

} // namespace Git
