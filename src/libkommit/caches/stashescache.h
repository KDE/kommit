/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QSharedPointer>
#include <git2/types.h>

#include "abstractcache.h"
#include "libkommit_export.h"

namespace Git
{
class Stash;
class LIBKOMMIT_EXPORT StashesCache
{
public:
    explicit StashesCache(Manager *manager);

    bool create(const QString &name = QString());
    Q_REQUIRED_RESULT bool apply(QSharedPointer<Stash> stash);
    Q_REQUIRED_RESULT bool pop(QSharedPointer<Stash> stash);
    Q_REQUIRED_RESULT bool remove(QSharedPointer<Stash> stash);
    Q_REQUIRED_RESULT bool drop(QSharedPointer<Stash> stash);

    QList<QSharedPointer<Stash>> allStashes();

    void clear();

private:
    Manager *manager;
};

};
