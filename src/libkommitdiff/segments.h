/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include "types.h"

#include <QStringList>

namespace Diff
{

struct LIBKOMMITDIFF_EXPORT SegmentRange {
    int start;
    int size;
};

struct LIBKOMMITDIFF_EXPORT Segment {
    virtual ~Segment() = default;

    QStringList oldText;
    QStringList newText;
    SegmentType type;

    virtual QStringList get(int index);

    virtual int count() const{
        return 2;
    }
    virtual int maxLines() const {
        return 0;
    }
};

struct LIBKOMMITDIFF_EXPORT DiffSegment : Segment {
    ~DiffSegment() override = default;

    int oldLineStart{};
    int oldLineEnd{};
    int newLineStart{};
    int newLineEnd{};

    SegmentRange left;
    SegmentRange right;

    [[nodiscard]] QStringList get(int index) override;
};

struct LIBKOMMITDIFF_EXPORT MergeSegment : Segment {
    ~MergeSegment() override = default;
    QStringList base;
    QStringList local;
    QStringList remote;
    //    MergeDiffType type;
    MergeType mergeType{None};

    [[nodiscard]] QStringList get(int index) override;
    MergeSegment();
    MergeSegment(const QStringList &base, const QStringList &local, const QStringList &remote);
};
}
