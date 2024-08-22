/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <git2/types.h>

#include "abstractcache.h"
#include "entities/stash.h"
#include "libkommit_export.h"

namespace Git
{

class StashesCachePrivate;

class LIBKOMMIT_EXPORT StashesCache
{
public:
    explicit StashesCache(Manager *manager);

    [[nodiscard]] QList<QSharedPointer<Stash>> allStashes();

    [[nodiscard]] QSharedPointer<Stash> findByName(const QString &name);
    [[nodiscard]] int findIndex(const QString &message) const;

    bool create(const QString &name = QString());

    bool apply(QSharedPointer<Stash> stash);
    bool pop(QSharedPointer<Stash> stash);
    bool remove(QSharedPointer<Stash> stash);

    bool apply(const QString &name);
    bool pop(const QString &name);
    bool remove(const QString &name);

    void clear();

private:
    StashesCachePrivate *d_ptr;
    Q_DECLARE_PRIVATE(StashesCache)
};
};
