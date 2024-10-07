/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "results.h"
#include <QDebug>

namespace Diff
{

QDebug operator<<(QDebug debug, const MergeSegment2 &ms)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "MergeSegment2("
                    << "base: (" << (ms.baseStart + 1) << ", " << ms.baseSize << ") local: (" << (ms.localStart + 1) << ", " << ms.localSize << ") remote: ("
                    << (ms.remoteStart + 1) << ", " << ms.remoteSize << "), type: " << ms.typeString() << ")";
    return debug;
}

QString MergeSegment2::typeString() const
{
    switch (type) {
    case Type::Unchanged:
        return "Unchanged";
    case Type::OnlyOnLocal:
        return "OnlyOnLocal";
    case Type::OnlyOnRemote:
        return "OnlyOnRemote";
    case Type::ChangedOnBoth:
        return "ChangedOnBoth";
    default:
        return "Unknown";
    }
}

int MergeSegment2::count() const
{
    return 3;
}

int MergeSegment2::maxLines() const
{
    return std::max(std::max(baseSize, localSize), std::max(baseSize, remoteSize));
}
}
