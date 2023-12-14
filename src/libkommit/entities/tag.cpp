/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tag.h"

#include <git2/commit.h>
#include <git2/tag.h>

namespace Git
{

Tag::Tag() = default;

Tag::Tag(git_tag *tag)
    : mTagPtr{tag}
{
    mName = git_tag_name(tag);
    mMessage = QString{git_tag_message(tag)}.replace("\n", "");
    auto tagger = git_tag_tagger(tag);

    mTagger.reset(new Signature{tagger});
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
    return mTagger;
}

QSharedPointer<Commit> Tag::commit() const
{
    auto type = git_tag_target_type(mTagPtr);

    if (type != GIT_OBJECT_COMMIT)
        return {};

    git_commit *commit;
    auto id = git_tag_target_id(mTagPtr);
    if (git_commit_lookup(&commit, git_tag_owner(mTagPtr), id))
        return {};

    return QSharedPointer<Commit>(new Commit{commit});
}
}
