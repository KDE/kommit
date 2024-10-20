/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcache.h"
#include "entities/stash.h"
#include "libkommit_export.h"

#include <git2/types.h>

#include <QObject>
#include <QScopedPointer>
#include <QSharedPointer>

namespace Git
{

class StashesCachePrivate;

class LIBKOMMIT_EXPORT StashesCache : public QObject
{
    Q_OBJECT

public:
    using DataType = Stash;
    using ListType = QList<Stash>;

    explicit StashesCache(Repository *manager);

    [[nodiscard]] QList<Stash> allStashes();

    [[nodiscard]] Stash findByName(const QString &name);
    [[nodiscard]] int findIndex(const QString &message) const;

    Q_REQUIRED_RESULT bool create(const QString &name = QString());

    Q_REQUIRED_RESULT bool apply(const Stash &stash);
    Q_REQUIRED_RESULT bool pop(const Stash &stash);
    Q_REQUIRED_RESULT bool remove(const Stash &stash);

    Q_REQUIRED_RESULT bool apply(const QString &name);
    Q_REQUIRED_RESULT bool pop(const QString &name);
    Q_REQUIRED_RESULT bool remove(const QString &name);

    void clear();

Q_SIGNALS:
    void added(const Stash &stash);
    void removed(const Stash &stash);
    void reloadRequired();

private:
    QScopedPointer<StashesCachePrivate> d_ptr;
    Q_DECLARE_PRIVATE(StashesCache)
};

}
