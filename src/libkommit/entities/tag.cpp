/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tag.h"

#include <git2/tag.h>

namespace Git
{

Tag::Tag() = default;

Tag::Tag(git_tag *tag)
{
    mName = git_tag_name(tag);
    mMessage = QString{git_tag_message(tag)}.replace("\n", "");
    auto tagger = git_tag_tagger(tag);
    mTaggerName = tagger->name;
    mTaggerEmail = tagger->email;
    mCreateTime = QDateTime::fromSecsSinceEpoch(tagger->when.time);
}

const QString &Tag::name() const
{
    return mName;
}

void Tag::setName(const QString &newName)
{
    mName = newName;
}

const QString &Tag::message() const
{
    return mMessage;
}

void Tag::setMessage(const QString &newMessage)
{
    mMessage = newMessage;
}

const QString &Tag::taggerEmail() const
{
    return mTaggerEmail;
}

void Tag::setTaggerEmail(const QString &newTaggerEmail)
{
    mTaggerEmail = newTaggerEmail;
}

const QString &Tag::taggerName() const
{
    return mTaggerName;
}

void Tag::setTaggerName(const QString &newTaggerName)
{
    mTaggerName = newTaggerName;
}

const QString &Tag::commiterName() const
{
    return mCommiterName;
}

void Tag::setCommiterName(const QString &newCommiterName)
{
    mCommiterName = newCommiterName;
}

const QString &Tag::commiterEmail() const
{
    return mCommiterEmail;
}

void Tag::setCommiterEmail(const QString &newCommiterEmail)
{
    mCommiterEmail = newCommiterEmail;
}

QDateTime Tag::createTime() const
{
    return mCreateTime;
}
}
