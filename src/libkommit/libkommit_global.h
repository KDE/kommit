/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

namespace Git
{

class Branch;
class Commit;
class Tag;
class Remote;

enum class BranchType {
    LocalBranch,
    RemoteBranch,
    AllBranches,
};
enum class ChangeStatus {
    Unknown,
    Unmodified,
    Modified,
    Added,
    Removed,
    Renamed,
    Copied,
    UpdatedButInmerged,
    Ignored,
    Untracked,
    TypeChange,
    Unreadable,
    Conflicted,
};

enum class ConflictSide {
    Base,
    Ours,
    Thers
};
}
