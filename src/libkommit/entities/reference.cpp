/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reference.h"
#include "caches/branchescache.h"
#include "caches/notescache.h"
#include "caches/remotescache.h"
#include "caches/tagscache.h"
#include "entities/branch.h"
#include "entities/note.h"
#include "entities/remote.h"
#include "entities/tag.h"
#include "oid.h"
#include "qdebug.h"
#include "repository.h"

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

Reference::Type Reference::type() const
{
    return static_cast<Type>(git_reference_type(ptr));
}

QSharedPointer<Oid> Reference::target() const
{
    auto oid = git_reference_target(ptr);
    return QSharedPointer<Oid>{new Oid{oid}};
}

QSharedPointer<Object> Reference::peel(Object::Type type) const
{
    git_object *object;

    if (git_reference_peel(&object, ptr, static_cast<git_object_t>(type)))
        return QSharedPointer<Object>{};
    return QSharedPointer<Object>{new Object{object}};
}

QSharedPointer<Note> Reference::toNote() const
{
    if (!isNote())
        return {};

    auto oid = git_reference_target(ptr);
    auto manager = Repository::owner(git_reference_owner(ptr));

    return manager->notes()->findByOid(oid);
}

QSharedPointer<Branch> Reference::toBranch() const
{
    if (!isBranch())
        return {};

    auto manager = Repository::owner(git_reference_owner(ptr));

    // memory leak warning: both branch and this will free the ptr!
    return manager->branches()->findByPtr(ptr);
}

QSharedPointer<Tag> Reference::toTag() const
{
    if (!isTag())
        return {};

    auto oid = git_reference_target(ptr);
    auto manager = Repository::owner(git_reference_owner(ptr));

    return manager->tags()->findByOid(oid);
}

QSharedPointer<Remote> Reference::toRemote() const
{
    if (!isBranch())
        return {};

    auto manager = Repository::owner(git_reference_owner(ptr));

    return manager->remotes()->findByName(name());
}

bool Reference::isValidName(const QString &name)
{
    return git_reference_is_valid_name(name.toUtf8().constData()) == 1;
}

git_reference *Reference::refPtr() const
{
    return ptr;
}
}
