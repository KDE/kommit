/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitgraphlane.h"

namespace Git
{

GraphLane::GraphLane()
{
    generateRandomColor();
}

GraphLane::Type GraphLane::type() const
{
    return mType;
}

void GraphLane::generateRandomColor()
{
    //    auto r = QRandomGenerator::global();
    //    _color = QColor(r->bounded(0, 150), r->bounded(0, 150), r->bounded(0, 150));
}

void GraphLane::setType(Type newType)
{
    mType = newType;
}

const QList<int> &GraphLane::upJoins() const
{
    return mUpJoins;
}

const QList<int> &GraphLane::bottomJoins() const
{
    return mBottomJoins;
}

GraphLane::GraphLane(GraphLane::Type type)
    : mType(type)
{
    generateRandomColor();
}

bool operator==(const GraphLane &p1, const GraphLane &p2)
{
    if (p1.type() == p2.type() && p1.type() == GraphLane::None)
        return true;

    return p1.type() == p2.type();
}

}
