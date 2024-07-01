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
#include "libkommit_export.h"

namespace Git
{

class Remote;

class LIBKOMMIT_EXPORT RemotesCache : public AbstractCache<Remote>
{
public:
    explicit RemotesCache(git_repository *repo);

protected:
    Remote *lookup(const QString &key, git_repository *repo) override;
};

};
