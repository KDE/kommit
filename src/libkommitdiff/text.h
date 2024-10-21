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
    QList<QString> lines;
    LineEnding lineEnding;
};

struct LIBKOMMITDIFF_EXPORT TextView {
    explicit TextView(const QString &content);
    QString content;
    QList<QStringView> lines;
    LineEnding lineEnding;
};

[[nodiscard]] Text readLines(const QString &text);
}
