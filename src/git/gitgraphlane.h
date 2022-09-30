/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include <QList>

namespace Git
{

namespace Impl
{
struct LanesFactory;
}

class LIBGITKLIENT_EXPORT GraphLane
{
public:
    enum Type { None, Start, Pipe, Node, End, Transparent, Test };
    GraphLane();
    GraphLane(Type type);

    Type type() const;
    const QList<int> &bottomJoins() const;
    const QList<int> &upJoins() const;
    void setType(Type newType);

private:
    void generateRandomColor();

    Type mType{None};
    QList<int> mBottomJoins;
    QList<int> mUpJoins;

    friend class LogList;
    friend struct LanesFactory;
    friend struct Impl::LanesFactory;
};
bool operator==(const GraphLane &, const GraphLane &);

}
