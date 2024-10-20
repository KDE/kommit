/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tag.h"

#include "oid.h"

#include "gitglobal_p.h"

#include <git2/commit.h>
#include <git2/refs.h>
#include <git2/tag.h>

namespace Git
{

class TagPrivate
{
public:
    explicit TagPrivate(git_tag *tag = nullptr);
    TagPrivate(git_commit *commit, const QString &name);
    explicit TagPrivate(Commit *parentCommit);
    ~TagPrivate();
    git_tag *tag{nullptr};

    Commit mLightTagCommit;

    Tag::Type mTagType;
    Signature mTagger;

    QString mName;
    QString mMessage;
};

TagPrivate::TagPrivate(git_tag *tag)
    : tag{tag}
    , mTagType{Tag::Type::RegularTag}
{
    if (tag) {
        mName = git_tag_name(tag);
        mMessage = QString{git_tag_message(tag)}.remove(QLatin1Char('\n'));

        auto tagger = git_tag_tagger(tag);
        mTagger = Signature{tagger};
    }
}

TagPrivate::TagPrivate(git_commit *commit, const QString &name)
    : mTagType{Tag::Type::LightTag}
    , mLightTagCommit{commit}
    , mName{name}
{
}

TagPrivate::TagPrivate(Commit *parentCommit)
    : mTagType{Tag::Type::LightTag}
    , mLightTagCommit{*parentCommit}

{
}

TagPrivate::~TagPrivate()
{
    git_tag_free(tag);
}

Tag::Tag(git_tag *tag)
    : d{new TagPrivate{tag}}
{
}

Tag::Tag(git_commit *commit, const QString &name)
    : d{new TagPrivate{commit, name}}
{
}

Tag::Tag(Commit *parentCommit)
    : d{new TagPrivate{parentCommit}}
{
}

Tag::Tag(const Tag &other)
    : d{other.d}
{
}

Tag &Tag::operator=(const Tag &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Tag::operator==(const Tag &other) const
{
    return d->tag == other.d->tag;
}

bool Tag::operator!=(const Tag &other) const
{
    return !(*this == other);
}

git_tag *Tag::data() const
{
    return d->tag;
}

const git_tag *Tag::constData() const
{
    return d->tag;
}

bool Tag::isNull() const
{
    return !d->tag;
}

const QString &Tag::name() const
{
    return d->mName;
}

const QString &Tag::message() const
{
    return d->mMessage;
}

const Signature &Tag::tagger() const
{
    if (d->mTagType == Type::LightTag)
        return d->mLightTagCommit.author();

    return d->mTagger;
}

Commit Tag::commit() const
{
    if (d->mTagType == Type::LightTag)
        return d->mLightTagCommit;

    auto type = git_tag_target_type(d->tag);

    if (type != GIT_OBJECT_COMMIT)
        return Commit{};

    git_commit *commit;
    auto id = git_tag_target_id(d->tag);
    if (git_commit_lookup(&commit, git_tag_owner(d->tag), id))
        return Commit{};

    return Commit{commit};
}

Tag::Type Tag::type() const
{
    return d->mTagType;
}

Oid Tag::oid() const
{
    return Oid{git_tag_id(d->tag)};
}

Object Tag::target() const
{
    git_object *target;
    if (git_tag_target(&target, d->tag))
        return Object{};
    return Object{target};
}
}
