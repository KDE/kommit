/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

#include "abstractcache.h"
#include "entities/commit.h"
#include "libkommit_export.h"

#include <git2/revwalk.h>
#include <git2/types.h>

#include <Kommit/CommitOptions>

namespace Git
{

class Branch;

class LIBKOMMIT_EXPORT CommitsCache : public QObject, public OidCache<Commit, git_commit>
{
    Q_OBJECT

public:
    enum SortingModeFlag {
        None = GIT_SORT_NONE,
        Topological = GIT_SORT_TOPOLOGICAL,
        Time = GIT_SORT_TIME,
        Reverse = GIT_SORT_REVERSE
    };
    Q_DECLARE_FLAGS(SortingMode, SortingModeFlag)
    // Q_FLAG(SortingMode)

    explicit CommitsCache(Repository *parent);

    [[nodiscard]] Commit find(const QString &hash);

    [[nodiscard]] QList<Commit> allCommits(SortingMode sortMode = static_cast<SortingMode>(Topological | Time));
    [[nodiscard]] QList<Commit> commitsInBranch(const Branch &branch);

    [[nodiscard]] bool create(const QString &message, const CommitOptions &options = {});
    [[nodiscard]] bool amend(const QString &message, const CommitOptions &options = {});

protected:
    void clearChildData() override;

Q_SIGNALS:
    void added(Git::Commit commit);
    void removed(Git::Commit commit);
    void reloadRequired();
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Git::CommitsCache::SortingMode)
