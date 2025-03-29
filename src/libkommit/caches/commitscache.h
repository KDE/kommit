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

#include <Kommit/CommitOptions>

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

    [[nodiscard]] bool create(const QString &message, const CommitOptions &options = {});
    [[nodiscard]] bool amend(const QString &message, const CommitOptions &options = {});

protected:
    void clearChildData() override;

Q_SIGNALS:
    void added(Commit commit);
    void removed(Commit commit);
    void reloadRequired();
};
}
