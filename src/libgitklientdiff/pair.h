/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QDebug>
#include <QPair>

namespace Diff
{
using Pair2 = QPair<int, int>;
struct Pair3 {
    int first;
    int second;
    int third;

    Pair3();
    Pair3(int firstNumber, int secondNumber, int thirdNumber);

    bool operator==(Diff::Pair3 other) const;
};

QDebug operator<<(QDebug d, Diff::Pair3 p);

}
