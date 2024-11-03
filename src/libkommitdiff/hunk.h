/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include "solution.h"

namespace Diff
{

// struct LIBKOMMITDIFF_EXPORT Range {
//     int start;
//     int size;
// };

class LIBKOMMITDIFF_EXPORT Hunk
{
public:
    Hunk();
    virtual ~Hunk();

    virtual int count() const = 0;
    virtual Range get(int index) const = 0;
};

class LIBKOMMITDIFF_EXPORT DiffHunk : public Hunk
{
public:
    DiffHunk();
    Range left;
    Range right;
    SegmentType type;

    int count() const override;
    Range get(int index) const override;
};

class LIBKOMMITDIFF_EXPORT MergeHunk : public Hunk
{
public:
    MergeHunk();
    Range base;
    Range local;
    Range remote;

    int count() const override;
    Range get(int index) const override;
};

}
