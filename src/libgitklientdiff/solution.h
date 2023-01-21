/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "pair.h"
#include "types.h"

namespace Diff
{

using Solution = QList<Pair2>;
using Solution3 = QList<Pair3>;

QDebug &operator<<(QDebug &stream, const Diff::Solution &sln);

class SolutionIterator
{
    const Solution &_solution;
    int _firstIndex{0};
    int _secondIndex{0};
    Solution::ConstIterator i;

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

    SolutionIterator(const Solution &solution);
    void begin();
    Result pick();
};

QDebug operator<<(QDebug d, const SolutionIterator::Result &r);
}
