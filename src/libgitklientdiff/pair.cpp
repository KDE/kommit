/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "pair.h"

namespace Diff
{

Pair3::Pair3()
    : first{-1}
    , second{-1}
    , third{-1}
{
}

Pair3::Pair3(int firstNumber, int secondNumber, int thirdNumber)
    : first{firstNumber}
    , second{secondNumber}
    , third{thirdNumber}
{
}

bool Pair3::operator==(Pair3 other) const
{
    return first == other.first && second == other.second && third == other.third;
}

QDebug operator<<(QDebug d, Pair3 p)
{
    d.noquote() << "(" << p.first << "," << p.second << "," << p.third << ")";
    return d;
}
}
