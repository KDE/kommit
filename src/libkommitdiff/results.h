/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include "segments.h"
#include "types.h"

#include <QList>

namespace Diff
{
struct LIBKOMMITDIFF_EXPORT Diff2Result {
    LineEnding oldTextLineEnding;
    LineEnding newTextLineEnding;
    QList<DiffSegment *> segments;
};

struct LIBKOMMITDIFF_EXPORT Diff3Result {
    LineEnding baseTextLineEnding;
    LineEnding localTextLineEnding;
    LineEnding remoteTextLineEnding;
    QList<MergeSegment *> segments;
};
};
