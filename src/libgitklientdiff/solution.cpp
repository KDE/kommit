/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "solution.h"

namespace Diff
{

SolutionIterator::Result::Result()
    : oldStart(0)
    , oldSize(0)
    , newStart(0)
    , newSize(0)
    , success(false)
    , type(SegmentType::DifferentOnBoth)
{
}

SolutionIterator::Result::Result(int oldStart, int oldSize, int newStart, int newSize, bool success, SegmentType type)
    : oldStart(oldStart)
    , oldSize(oldSize)
    , newStart(newStart)
    , newSize(newSize)
    , success(success)
    , type(type)
{
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

        return {fi, _firstIndex - fi, si, _secondIndex - si, true, SegmentType::SameOnBoth};

    } else {
        Result r{_firstIndex, i->first - _firstIndex, _secondIndex, i->second - _secondIndex, true, SegmentType::DifferentOnBoth};
        _firstIndex = i->first;
        _secondIndex = i->second;

        if (r.newSize && r.oldSize)
            r.type = SegmentType::DifferentOnBoth;
        else if (r.newSize)
            r.type = SegmentType::OnlyOnRight;
        else
            r.type = SegmentType::OnlyOnLeft;
        r.success = true;
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

QDebug operator<<(QDebug d, const SolutionIterator::Result &r)
{
    d.noquote() << "(Old: " << r.oldStart << " size=" << r.oldSize << ", New: " << r.newStart << " size=" << r.newSize << ")";
    return d;
}

SolutionIterator3::SolutionIterator3(const Solution3 &solution)
    : _solution(solution)
    , i(_solution.begin())
{
}

void SolutionIterator3::begin()
{
    _firstIndex = _secondIndex = _thirdIndex = -1;
    i = _solution.begin();
}

Diff::SolutionIterator3::Result Diff::SolutionIterator3::pick()
{
    if (i == _solution.end())
        return {};

    if ((i->first == _firstIndex && i->second == _secondIndex && i->third == _thirdIndex)) {
        auto fi = _firstIndex;
        auto si = _secondIndex;
        auto ti = _thirdIndex;
        do {
            _firstIndex++;
            _secondIndex++;
            _thirdIndex++;
            ++i;
        } while (i != _solution.end() && i->first == _firstIndex && i->second == _secondIndex);

        return {{fi, _firstIndex - fi}, {si, _secondIndex - si}, {ti, _thirdIndex - ti}, SegmentType::SameOnBoth};

    } else {
        Result r{{_firstIndex, i->first - _firstIndex},
                 {_secondIndex, i->second - _secondIndex},
                 {_thirdIndex, i->third - _thirdIndex},
                 SegmentType::DifferentOnBoth};
        _firstIndex = i->first;
        _secondIndex = i->second;
        _thirdIndex = i->third;

        if (r.local.size && r.remote.size)
            r.type = SegmentType::DifferentOnBoth;
        else if (r.remote.size)
            r.type = SegmentType::OnlyOnRight;
        else
            r.type = SegmentType::OnlyOnLeft;
        r.success = true;
        return r;
    }

    return {};
}

SolutionIterator3::Result::Result()
    : base{}
    , local{}
    , remote{}
    , success{false}
    , type{}
{
}

Diff::SolutionIterator3::Result::Result(Range base, Range local, Range remote, SegmentType type)
    : base{base}
    , local{local}
    , remote{remote}
    , success{true}
    , type{type}
{
}

Range::Range()
    : begin{-1}
    , size{-1}
{
}

Range::Range(int begin, int size)
    : begin{begin}
    , size{size}
{
}
}
