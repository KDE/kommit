/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include "abstractcache.h"
#include "entities/commit.h"
#include "libkommit_export.h"

#include <git2/types.h>

namespace Git
{

class Branch;

class LIBKOMMIT_EXPORT CommitsCache : public QObject, public OidCache<Commit, git_commit>
{
    Q_OBJECT

public:
    explicit CommitsCache(Manager *parent);

    Q_REQUIRED_RESULT QSharedPointer<Commit> find(const QString &hash);

    Q_REQUIRED_RESULT QList<QSharedPointer<Commit>> allCommits();
    Q_REQUIRED_RESULT QList<QSharedPointer<Commit>> commitsInBranch(QSharedPointer<Branch> branch);

protected:
    void clearChildData() override;

Q_SIGNALS:
    void added(DataMember commit);
    void removed(DataMember commit);
};
}
