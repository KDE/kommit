/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reference.h"
#include "entities/branch.h"
#include "entities/note.h"
#include "entities/remote.h"
#include "entities/tag.h"
#include <git2/notes.h>
#include <git2/refs.h>
#include <git2/remote.h>
#include <git2/tag.h>

namespace Git
{

Reference::Reference()
{
}

Reference::Reference(git_reference *ref)
    : ptr{ref}
{
    mIsNote = git_reference_is_note(ref);
    mIsBranch = git_reference_is_branch(ref);
    mIsRemote = git_reference_is_remote(ref);
    mIsTag = git_reference_is_tag(ref);
    mName = git_reference_name(ref);
    mShorthand = git_reference_shorthand(ref);
}

Reference::~Reference()
{
    git_reference_free(ptr);
    ptr = nullptr;
}

bool Reference::isNote() const
{
    return mIsNote;
}

bool Reference::isBranch() const
{
    return mIsBranch;
}

bool Reference::isTag() const
{
    return mIsTag;
}

bool Reference::isRemote() const
{
    return mIsRemote;
}

QString Reference::name() const
{
    return mName;
}

QString Reference::shorthand() const
{
    return mShorthand;
}

QSharedPointer<Note> Reference::toNote() const
{
    if (!isNote())
        return {};

    auto oid = git_reference_target(ptr);
    auto repo = git_reference_owner(ptr);

    git_note *note;
    git_note_read(&note, repo, NULL, oid);

    return QSharedPointer<Note>{new Note{note}};
}

QSharedPointer<Branch> Reference::toBranch() const
{
    if (!isBranch())
        return {};

    return QSharedPointer<Branch>{new Branch{ptr}};
}

QSharedPointer<Tag> Reference::toTag() const
{
    if (!isTag())
        return {};

    auto oid = git_reference_target(ptr);
    auto repo = git_reference_owner(ptr);

    git_tag *tag;
    git_tag_lookup(&tag, repo, oid);

    return QSharedPointer<Tag>{new Tag{tag}};
}

QSharedPointer<Remote> Reference::toRemote() const
{
    if (!isBranch())
        return {};

    auto repo = git_reference_owner(ptr);

    git_remote *remote;
    git_remote_lookup(&remote, repo, git_reference_name(ptr));

    return QSharedPointer<Remote>{new Remote{remote}};
}
}
