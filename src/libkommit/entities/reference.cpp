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

class ReferencePrivate
{
public:
    explicit ReferencePrivate(git_reference *ref = nullptr);
    ~ReferencePrivate();
    git_reference *reference{nullptr};
};

ReferencePrivate::ReferencePrivate(git_reference *ref)
    : reference{ref}
{
}

ReferencePrivate::~ReferencePrivate()
{
    git_reference_free(reference);
}

Reference::Reference()
    : d{new ReferencePrivate}
{
}

Reference::Reference(git_reference *ref)
    : d{new ReferencePrivate{ref}}
{
}

Reference::Reference(const Reference &other)
    : d{other.d}
{
}

Reference &Reference::operator=(const Reference &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Reference::operator==(const Reference &other) const
{
    return d->reference == other.d->reference;
}

bool Reference::operator!=(const Reference &other) const
{
    return !(*this == other);
}

bool Reference::isNull() const
{
    return !d->reference;
}

git_reference *Reference::data() const
{
    return d->reference;
}

const git_reference *Reference::constData() const
{
    return d->reference;
}

bool Reference::isNote() const
{
    return git_reference_is_note(d->reference);
}

bool Reference::isBranch() const
{
    return git_reference_is_branch(d->reference);
}

bool Reference::isTag() const
{
    return git_reference_is_tag(d->reference);
}

bool Reference::isRemote() const
{
    return git_reference_is_remote(d->reference);
}

QString Reference::name() const
{
    return QString{git_reference_name(d->reference)};
}

QString Reference::shorthand() const
{
    return QString{git_reference_shorthand(d->reference)};
}

Reference::Type Reference::type() const
{
    return static_cast<Type>(git_reference_type(d->reference));
}

Oid Reference::target() const
{
    auto oid = git_reference_target(d->reference);
    return Oid{oid};
}

Object Reference::peel(Object::Type type) const
{
    git_object *object;

    if (git_reference_peel(&object, d->reference, static_cast<git_object_t>(type)))
        return Object{};
    return Object{object};
}

Note Reference::toNote() const
{
    if (!isNote())
        return Note{};

    auto oid = git_reference_target(d->reference);
    auto manager = Repository::owner(git_reference_owner(d->reference));

    return manager->notes()->findByOid(oid);
}

Branch Reference::toBranch() const
{
    if (!isBranch())
        return {};

    auto manager = Repository::owner(git_reference_owner(d->reference));

    // memory leak warning: both branch and this will free the d->reference!
    return manager->branches()->findByPtr(d->reference);
}

Tag Reference::toTag() const
{
    if (!isTag())
        return {};

    auto oid = git_reference_target(d->reference);
    auto manager = Repository::owner(git_reference_owner(d->reference));

    return manager->tags()->findByOid(oid);
}

Remote Reference::toRemote() const
{
    if (!isBranch())
        return {};

    auto manager = Repository::owner(git_reference_owner(d->reference));

    return manager->remotes()->findByName(name());
}

bool Reference::isValidName(const QString &name)
{
    return git_reference_is_valid_name(name.toUtf8().constData()) == 1;
}

git_reference *Reference::refPtr() const
{
    return d->reference;
}
}
