/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stash.h"

#include "caches/commitscache.h"
#include "entities/commit.h"
#include "entities/oid.h"
#include "gitmanager.h"

#include <git2/stash.h>

namespace Git
{

class StashPrivate
{
    Stash *q_ptr;
    Q_DECLARE_PUBLIC(Stash)

public:
    StashPrivate(Stash *parent);
    Manager *manager;
    size_t index;
    QString message;
    git_oid stash_id;

    QSharedPointer<Commit> commit;
};

Stash::Stash(Manager *manager, size_t index, const char *message, const git_oid *stash_id)
    : d_ptr{new StashPrivate{this}}
{
    Q_D(Stash);

    git_oid oid_cpy;
    git_oid_cpy(&oid_cpy, stash_id);

    d->index = index;
    d->stash_id = oid_cpy;
    d->message = QString{message};
    d->manager = manager;
}

Stash::~Stash()
{
    Q_D(Stash);

    delete d;
}

const QString &Stash::message() const
{
    Q_D(const Stash);
    return d->message;
}

QSharedPointer<Commit> Stash::commit()
{
    Q_D(Stash);

    if (d->commit.isNull()) {
        d->commit = d->manager->commits()->findByOid(&d->stash_id);
    }
    return d->commit;
}

size_t Stash::index() const
{
    Q_D(const Stash);
    return d->index;
}

QSharedPointer<Oid> Stash::oid() const
{
    Q_D(const Stash);
    return QSharedPointer<Oid>{new Oid{&d->stash_id}};
}

StashPrivate::StashPrivate(Stash *parent)
    : q_ptr{parent}
{
}

} // namespace Git
