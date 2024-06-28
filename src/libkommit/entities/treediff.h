/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QList>

#include "gitglobal.h"
#include "libkommit_export.h"

#include <git2/diff.h>

namespace Git
{

class LIBKOMMIT_EXPORT TreeDiffEntry
{
public:
    TreeDiffEntry();
    explicit TreeDiffEntry(const git_diff_delta *delta);

    Q_REQUIRED_RESULT git_diff_delta *deltaPtr() const;
    Q_REQUIRED_RESULT QString oldFile() const;
    Q_REQUIRED_RESULT QString newFile() const;
    Q_REQUIRED_RESULT ChangeStatus status() const;

    bool operator==(const TreeDiffEntry &other) const;

private:
    git_diff_delta *mDeltaPtr{nullptr};

    QString mOldFile;
    QString mNewFile;
    ChangeStatus mStatus;
};

class LIBKOMMIT_EXPORT TreeDiff : public QList<TreeDiffEntry>
{
public:
    TreeDiff();
    TreeDiff(git_diff *diff);
    ~TreeDiff();

    bool contains(const QString &entryPath) const;
    ChangeStatus status(const QString &entryPath) const;

private:
    git_diff *mDiff{nullptr};
};

} // namespace Git
