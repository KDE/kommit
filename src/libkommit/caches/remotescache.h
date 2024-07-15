/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QMap>
#include <QObject>
#include <QSharedPointer>
#include <git2/types.h>

#include "abstractcache.h"
#include "entities/remote.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT RemotesCache : public Cache<Remote, git_remote>
{
public:
    explicit RemotesCache(Manager *manager);

    QList<QSharedPointer<Remote>> allRemotes();
    QStringList allNames();
    QSharedPointer<Remote> findByName(const QString &name);
    Q_REQUIRED_RESULT bool create(const QString &name, const QString &url);
    Q_REQUIRED_RESULT bool setUrl(DataMember remote, const QString &url);

    Q_REQUIRED_RESULT bool remove(QSharedPointer<Remote> remote);
    Q_REQUIRED_RESULT bool remove(const QString &name) const;

    Q_REQUIRED_RESULT bool rename(QSharedPointer<Remote> remote, const QString &newName);

protected:
    void clearChildData() override;
};
};
