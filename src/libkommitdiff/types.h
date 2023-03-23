/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

namespace Diff
{

enum class LineEnding { None, Cr, Lf, CrLf };

enum class DiffType { Unchanged, Added, Removed, Modified };

enum MergeType { None, KeepLocal, KeepRemote, KeepLocalThenRemote, KeepRemoteThenLocal, MergeCustom };

enum class SegmentType {
    SameOnBoth,
    OnlyOnLeft,
    OnlyOnRight,
    DifferentOnBoth,
};

enum MergeDiffType { Unchanged, LocalAdd, RemoteAdd, BothChanged };

}
