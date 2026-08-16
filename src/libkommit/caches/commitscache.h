/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include "abstractcache.h"
#include "entities/commit.h"
#include "libkommit_export.h"

#include <git2/oid.h>
#include <git2/types.h>

namespace Git
{

class Branch;

class LIBKOMMIT_EXPORT CommitsCache : public QObject, public OidCache<Commit, git_commit>
{
    Q_OBJECT

public:
    explicit CommitsCache(Repository *parent);

    [[nodiscard]] Commit find(const QString &hash);

    [[nodiscard]] QList<Commit> allCommits();
    [[nodiscard]] QList<Commit> commitsInBranch(const Branch &branch);

    /**
     * Commit entities for @p oids, in the order given, with their child links and the
     * references pointing at them filled in.
     *
     * The entities and the cache holding them belong to one thread, so this is the half of
     * reading a history that cannot be moved away from the thread that owns the repository.
     * walkCommits() produces the object ids it takes.
     */
    [[nodiscard]] QList<Commit> commitsFromOids(const QList<git_oid> &oids);

protected:
    void clearChildData() override;

private:
    void linkCommits(QList<Commit> &list);

Q_SIGNALS:
    void added(DataType commit);
    void removed(DataType commit);
    void reloadRequired();
};
}
