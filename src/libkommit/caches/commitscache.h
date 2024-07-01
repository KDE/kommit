/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include "abstractcache.h"
#include "libkommit_export.h"
#include <git2/types.h>

namespace Git
{
class Commit;
class Branch;
class LIBKOMMIT_EXPORT CommitsCache : public QObject, public AbstractCache<Commit>
{
    Q_OBJECT
public:
    explicit CommitsCache(git_repository *repo);

    QList<QSharedPointer<Commit>> commitsInBranch(QSharedPointer<Branch> branch);

protected:
    Commit *lookup(const QString &key, git_repository *repo) override;
};
}
