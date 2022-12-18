/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "segments.h"

namespace Diff
{
QStringList Segment::get(int index)
{
    switch (index) {
    case 0:
        return oldText;
    case 1:
        return newText;
    }
    return {};
}

QStringList DiffSegment::get(int index)
{
    switch (index) {
    case 0:
        return oldText;
    case 1:
        return newText;
    }
    return {};
}

MergeSegment::MergeSegment() = default;

MergeSegment::MergeSegment(const QStringList &base, const QStringList &local, const QStringList &remote)
    : base{base}
    , local{local}
    , remote{remote}
{
    if (local == remote)
        type = SegmentType::SameOnBoth;
    else if (base.empty() && !local.empty())
        type = SegmentType::OnlyOnLeft;
    else if (base.empty() && !remote.empty())
        type = SegmentType::OnlyOnRight;
    else
        type = SegmentType::DifferentOnBoth;

    oldText = local;
    newText = remote;

    if (type == SegmentType::SameOnBoth)
        this->base = this->local;
}

QStringList MergeSegment::get(int index)
{
    switch (index) {
    case 0:
        return base;
    case 1:
        return local;
    case 2:
        return remote;
    }
    return {};
}

}
