/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QSharedPointer>

#include <git2/merge.h>

namespace Git
{

class MergeOptionsPrivate;
class LIBKOMMIT_EXPORT MergeOptions
{
    Q_GADGET

public:
    enum class Flag {
        Default = 0,
        FindRenames = GIT_MERGE_FIND_RENAMES,
        FailOnConflict = GIT_MERGE_FAIL_ON_CONFLICT,
        SkipReuc = GIT_MERGE_SKIP_REUC,
        NoRecursive = GIT_MERGE_NO_RECURSIVE,
        VirtualBase = GIT_MERGE_VIRTUAL_BASE,
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    enum class MergeFileFlag {
        Default = GIT_MERGE_FILE_DEFAULT,
        StyleMerge = GIT_MERGE_FILE_STYLE_MERGE,
        StyleDiff3 = GIT_MERGE_FILE_STYLE_DIFF3,
        SimplifyAlnum = GIT_MERGE_FILE_SIMPLIFY_ALNUM,
        IgnoreWhitespace = GIT_MERGE_FILE_IGNORE_WHITESPACE,
        IgnoreWhitespaceChange = GIT_MERGE_FILE_IGNORE_WHITESPACE_CHANGE,
        IgnoreWhitespaceEol = GIT_MERGE_FILE_IGNORE_WHITESPACE_EOL,
        DiffPatience = GIT_MERGE_FILE_DIFF_PATIENCE,
        DiffMinimal = GIT_MERGE_FILE_DIFF_MINIMAL,
        StyleZdiff3 = GIT_MERGE_FILE_STYLE_ZDIFF3,
        AcceptConflicts = GIT_MERGE_FILE_ACCEPT_CONFLICTS
    };
    Q_DECLARE_FLAGS(MergeFileFlags, MergeFileFlag)

    enum class FileFlavor {
        Normal = GIT_MERGE_FILE_FAVOR_NORMAL,
        Ours = GIT_MERGE_FILE_FAVOR_OURS,
        Theirs = GIT_MERGE_FILE_FAVOR_THEIRS,
        Union = GIT_MERGE_FILE_FAVOR_UNION
    };

    MergeOptions();

    void apply(git_merge_options *opts) const;

    [[nodiscard]] Flags flags() const;
    void setFlags(const Flags &flags);

    [[nodiscard]] MergeFileFlags mergeFileFlags() const;
    void setMergeFileFlags(const MergeFileFlags &flags);

    [[nodiscard]] FileFlavor fileFlavor() const;
    void setFileFlavor(FileFlavor fileFlavor);

    [[nodiscard]] unsigned int renameThreshold() const;
    void setRenameThreshold(unsigned int renameThreshold);

    [[nodiscard]] unsigned int targetLimit() const;
    void setTargetLimit(const unsigned int &targetLimit);

    [[nodiscard]] unsigned int recursionLimit() const;
    void setRecursionLimit(const unsigned int &recursionLimit);

private:
    QSharedPointer<MergeOptionsPrivate> d;
};

}
