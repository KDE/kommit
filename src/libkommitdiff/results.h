/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include "segments.h"
#include "text.h"
#include "types.h"

#include <QList>

namespace Diff
{

struct LIBKOMMITDIFF_EXPORT Diff2Result {
    Text left;
    Text right;
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

struct LIBKOMMITDIFF_EXPORT MergeSegmentRange {
    const int start;
    const int size;
};

struct LIBKOMMITDIFF_EXPORT MergeSegment2 : Segment {
    int baseStart;
    int baseSize;

    int localStart;
    int localSize;

    int remoteStart;
    int remoteSize;

    enum class Type { Unchanged, OnlyOnLocal, OnlyOnRemote, ChangedOnBoth };
    Type type;

    QString typeString() const;

    virtual int count() const;
    virtual int maxLines() const;
};

LIBKOMMITDIFF_EXPORT QDebug operator<<(QDebug debug, const MergeSegment2 &ms);

enum class MergeSide { Base, Local, Remote };

template<typename T>
struct MergeResult {
    QList<T> base;
    QList<T> local;
    QList<T> remote;

    QList<MergeSegment2 *> segments;

    QList<T> part(MergeSegment2 *segment, MergeSide side)
    {
        switch (side) {
        case MergeSide::Base:
            return base.mid(segment->baseStart, segment->baseSize);
        case MergeSide::Local:
            return local.mid(segment->localStart, segment->localSize);
        case MergeSide::Remote:
            return remote.mid(segment->remoteStart, segment->remoteSize);
        }
        return {};
    }
};

template<>
struct MergeResult<Text> {
    Text base;
    Text local;
    Text remote;

    QList<MergeSegment2 *> segments;

    QList<QString> part(MergeSegment2 *segment, MergeSide side)
    {
        switch (side) {
        case MergeSide::Base:
            return base.lines.mid(segment->baseStart, segment->baseSize);
        case MergeSide::Local:
            return local.lines.mid(segment->localStart, segment->localSize);
        case MergeSide::Remote:
            return remote.lines.mid(segment->remoteStart, segment->remoteSize);
        }
        return {};
    }
};
}
