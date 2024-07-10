/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tag.h"

#include "oid.h"

#include <git2/commit.h>
#include <git2/tag.h>

namespace Git
{

Tag::Tag() = default;

Tag::Tag(git_tag *tag)
    : mTagPtr{tag}
    , mTagType{TagType::RegularTag}
{
    mName = git_tag_name(tag);
    mMessage = QString{git_tag_message(tag)}.remove(QLatin1Char('\n'));
    auto tagger = git_tag_tagger(tag);

    mTagger.reset(new Signature{tagger});
}

Tag::Tag(git_commit *commit, const QString &name)
    : mTagType{TagType::LightTag}
    , mName{name}
{
    mLightTagCommit.reset(new Commit{commit});
}

Tag::Tag(Commit *parentCommit)
    : mLightTagCommit{parentCommit}
    , mTagType{TagType::LightTag}
{
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

QSharedPointer<Signature> Tag::tagger() const
{
    if (mTagType == TagType::LightTag)
        return mLightTagCommit->author();

    return mTagger;
}

QSharedPointer<Commit> Tag::commit() const
{
    if (mTagType == TagType::LightTag)
        return mLightTagCommit;

    auto type = git_tag_target_type(mTagPtr);

    if (type != GIT_OBJECT_COMMIT)
        return {};

    git_commit *commit;
    auto id = git_tag_target_id(mTagPtr);
    if (git_commit_lookup(&commit, git_tag_owner(mTagPtr), id))
        return {};

    return QSharedPointer<Commit>(new Commit{commit});
}

Tag::TagType Tag::tagType() const
{
    return mTagType;
}

QSharedPointer<Oid> Tag::oid() const
{
    return QSharedPointer<Oid>(new Oid{git_tag_id(mTagPtr)});
}

git_tag *Tag::tagPtr() const
{
    return mTagPtr;
}
}
