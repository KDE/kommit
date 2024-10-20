/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stash.h"

#include "caches/commitscache.h"
#include "entities/commit.h"
#include "entities/oid.h"
#include "oid.h"
#include "repository.h"

#include <git2/stash.h>

namespace Git
{

class StashPrivate
{
public:
    StashPrivate();
    StashPrivate(Repository *manager, size_t index, const char *message, const git_oid *stash_id);
    Repository *manager;
    size_t index;
    QString message;
    Oid stashId;
    bool isNull;

    Commit commit;
};

Stash::Stash() noexcept
    : d{new StashPrivate}
{
}

Stash::Stash(Repository *manager, size_t index, const char *message, const git_oid *stash_id)
    : d{new StashPrivate{manager, index, message, stash_id}}
{
}

Stash::Stash(const Stash &other)
    : d{other.d}
{
}

Stash &Stash::operator=(const Stash &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Stash::operator==(const Stash &other) const
{
    return d->stashId == other.d->stashId;
}

bool Stash::operator!=(const Stash &other) const
{
    return !(*this == other);
}

const QString &Stash::message() const
{
    return d->message;
}

Commit &Stash::commit()
{
    if (d->commit.isNull()) {
        d->commit = d->manager->commits()->findByOid(d->stashId.data());
    }
    return d->commit;
}

size_t Stash::index() const
{
    return d->index;
}

Oid Stash::oid() const
{
    return d->stashId;
}

bool Stash::isNull() const
{
    return d->isNull;
}

StashPrivate::StashPrivate()
    : manager{}
    , index{}
    , message{}
    , stashId{}
    , isNull{true}
{
}

StashPrivate::StashPrivate(Repository *manager, size_t index, const char *message, const git_oid *stash_id)
    : manager{manager}
    , index{index}
    , message{message}
    , stashId{stash_id}
    , isNull{false}
{
}

} // namespace Git
