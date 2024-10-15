/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <stddef.h>
#include <stdint.h>

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

enum class ConflictSide { Base, Ours, Thers };

struct FetchTransferStat {
    unsigned int totalObjects;
    unsigned int indexedObjects;
    unsigned int receivedObjects;
    unsigned int localObjects;
    unsigned int totalDeltas;
    unsigned int indexedDeltas;
    size_t receivedBytes;
};
struct PackProgress {
    int stage;
    uint32_t current;
    uint32_t total;
};

}

