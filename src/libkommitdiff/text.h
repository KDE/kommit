/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include "types.h"

#include <QList>
#include <QString>

namespace Diff
{
struct LIBKOMMITDIFF_EXPORT Text {
    Text();
    QString content;
    QList<QString> lines; // TODO: convert to QList<QStringRef> if it's possible
    LineEnding lineEnding;
};

[[nodiscard]] Text readLines(const QString &text);
}
