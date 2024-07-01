/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reference.h"
#include "entities/branch.h"
#include "entities/note.h"
#include "entities/remote.h"
#include "entities/tag.h"
#include "oid.h"

#include <git2/notes.h>
#include <git2/oid.h>
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
}

Reference::~Reference()
{
    git_reference_free(ptr);
    ptr = nullptr;
}

bool Reference::isNote() const
{
    return git_reference_is_note(ptr);
}

bool Reference::isBranch() const
{
    return git_reference_is_branch(ptr);
}

bool Reference::isTag() const
{
    return git_reference_is_tag(ptr);
}

bool Reference::isRemote() const
{
    return git_reference_is_remote(ptr);
}

QString Reference::name() const
{
    return QString{git_reference_name(ptr)};
}

QString Reference::shorthand() const
{
    return QString{git_reference_shorthand(ptr)};
}

QSharedPointer<Oid> Reference::target() const
{
    auto oid = git_reference_target(ptr);
    return QSharedPointer<Oid>{new Oid{oid}};
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
