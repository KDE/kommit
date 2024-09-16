/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "pair.h"
#include "types.h"

#include "libkommitdiff_export.h"

namespace Diff
{

using Solution = QList<Pair2>;
using Solution3 = QList<Pair3>;

QDebug &operator<<(QDebug &stream, const Diff::Solution &sln);

struct Range {
    int begin;
    int size;

    Range();
    Range(int begin, int size);
};

class LIBKOMMITDIFF_EXPORT SolutionIterator
{
    const Solution &_solution;
    int _firstIndex{0};
    int _secondIndex{0};
    Solution::ConstIterator i;
    bool _ended{true};
    int _firstSize{};
    int _secondSize{};

public:
    struct Result {
        int oldStart;
        int oldSize;
        int newStart;
        int newSize;
        bool success;
        SegmentType type;

        Result();
        Result(int oldStart, int oldSize, int newStart, int newSize, bool success, SegmentType type);
    };

    SolutionIterator(const Solution &solution, int firstSize, int secondSize);
    void begin();
    Result pick();
};

class LIBKOMMITDIFF_EXPORT SolutionIterator3
{
    const Solution3 &_solution;
    int _firstIndex{0};
    int _secondIndex{0};
    int _thirdIndex{0};
    Solution3::ConstIterator i;

public:
    struct Result {
        Range base;
        Range local;
        Range remote;
        bool success;
        SegmentType type;

        Result();
        Result(Range base, Range local, Range remote, SegmentType type);
    };

    SolutionIterator3(const Solution3 &solution);
    void begin();
    Result pick();
};

QDebug operator<<(QDebug d, const SolutionIterator::Result &r);

class LIBKOMMITDIFF_EXPORT SolutionIterator3_2
{
    Solution _withLocal;
    Solution _withRemote;
    int _baseSize;
    int _localSize;
    int _remoteSize;

    Solution::ConstIterator _iteratorWithLocal;
    Solution::ConstIterator _iteratorWithRemote;

public:
    struct Result {
        Range base;
        Range local;
        Range remote;
        bool success;
        SegmentType type;

        Result();
        Result(Range base, Range local, Range remote, SegmentType type);
    };

    SolutionIterator3_2(const Solution &withLocal, Solution withRemote, int baseSize, int localSize, int remoteSize)
        : _withLocal{withLocal}
        , _withRemote{withRemote}
        , _baseSize{baseSize}
        , _localSize{localSize}
        , _remoteSize{remoteSize}
    {
    }

    // void begin();
    // Result pick();
};
}
