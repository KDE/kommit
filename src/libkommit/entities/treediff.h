/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QList>

#include "libkommit_export.h"
#include "libkommit_global.h"
#include "types.h"

#include <git2/diff.h>

namespace Git
{

class LIBKOMMIT_EXPORT TreeDiffEntry
{
public:
    TreeDiffEntry();
    explicit TreeDiffEntry(const git_diff_delta *delta);

    [[nodiscard]] git_diff_delta *deltaPtr() const;
    [[nodiscard]] QString oldFile() const;
    [[nodiscard]] QString newFile() const;
    [[nodiscard]] DeltaFlag status() const;

    bool operator==(const TreeDiffEntry &other) const;

private:
    git_diff_delta *mDeltaPtr{nullptr};

    QString mOldFile;
    QString mNewFile;
    DeltaFlag mStatus;
};

class LIBKOMMIT_EXPORT TreeDiff : public QList<TreeDiffEntry>
{
public:
    TreeDiff();
    TreeDiff(git_diff *diff);
    ~TreeDiff();

    bool contains(const QString &entryPath) const;
    DeltaFlag status(const QString &entryPath) const;

private:
    git_diff *mDiff{nullptr};
};

} // namespace Git
