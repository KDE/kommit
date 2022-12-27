/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "types.h"

#include <QList>
#include <QString>

namespace Diff
{
struct Text {
    QList<QString> lines; // TODO: convert to QList<QStringRef> if it's possible
    LineEnding lineEnding;
};

Q_REQUIRED_RESULT Text readLines(const QString &text);
}
