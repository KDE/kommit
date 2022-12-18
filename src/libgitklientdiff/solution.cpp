/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "solution.h"

namespace Diff
{

SolutionIterator::Result::Result()
    : success(false)
{
}

SolutionIterator::Result::Result(int oldStart, int oldEnd, int newStart, int newEnd, bool success, SegmentType type)
    : oldStart(oldStart)
    , newStart(newStart)
    , oldEnd(oldEnd)
    , newEnd(newEnd)
    , success(success)
    , type(type)
{
}

int SolutionIterator::Result::oldSize() const
{
    return oldEnd - oldStart;
}

int SolutionIterator::Result::newSize() const
{
    return newEnd - newStart;
}

SolutionIterator::SolutionIterator(const Solution &solution)
    : _solution(solution)
    , i(_solution.begin())
{
}

void SolutionIterator::begin()
{
    _firstIndex = _secondIndex = -1;
    i = _solution.begin();
}

SolutionIterator::Result SolutionIterator::pick()
{
    if (i == _solution.end())
        return {};

    if ((i->first == _firstIndex && i->second == _secondIndex)) {
        auto fi = _firstIndex;
        auto si = _secondIndex;
        do {
            _firstIndex++;
            _secondIndex++;
            ++i;
        } while (i != _solution.end() && i->first == _firstIndex && i->second == _secondIndex);

        return {fi, _firstIndex, si, _secondIndex, true, SegmentType::SameOnBoth};
    } else {
        Result r{_firstIndex, i->first, _secondIndex, i->second, true, SegmentType::DifferentOnBoth};
        ++i;
        if (r.newStart == r.newEnd)
            r.type = SegmentType::OnlyOnLeft;
        else if (r.oldStart == r.oldEnd)
            r.type = SegmentType::OnlyOnRight;
        else
            r.type = SegmentType::DifferentOnBoth;
        _firstIndex = i->first;
        _secondIndex = i->second;
        return r;
    }

    return {};
}
QDebug &operator<<(QDebug &stream, const Diff::Solution &sln)
{
    for (auto i = sln.begin(); i != sln.end(); ++i)
        stream << "(" << i->first << ", " << i->second << ")";
    return stream;
}

}
