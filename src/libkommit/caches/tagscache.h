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

class Tag;

class LIBKOMMIT_EXPORT TagsCache : public AbstractCache<Tag>
{
public:
    explicit TagsCache(git_repository *repo);

protected:
    Tag *lookup(const QString &key, git_repository *repo) override;
};

};
