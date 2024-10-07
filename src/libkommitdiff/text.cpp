/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "text.h"

#include <QDebug>

namespace Diff
{

Text::Text()
    : lineEnding(LineEnding::None)
{
}

Text readLines(const QString &text)
{
    if (text.isEmpty())
        return {};

    LineEnding le{LineEnding::None};
    QString separator;
    for (const auto &ch : text) {
        if (le == LineEnding::Cr) {
            if (ch == QLatin1Char('\n')) {
                le = LineEnding::CrLf;
                separator = QStringLiteral("\r\n");
            }
            break;
        }
        if (ch == QLatin1Char('\r')) {
            le = LineEnding::Cr;
            separator = QStringLiteral("\r");
            continue;
        }
        if (ch == QLatin1Char('\n')) {
            separator = QStringLiteral("\n");
            le = LineEnding::Lf;
            break;
        }
    }

    if (le == LineEnding::None) {
        qWarning() << "Unable to detect line ending";
        return {};
    }

    int i{0};
    Text t;
    t.content = text;
    t.lineEnding = le;
    while (i != -1) {
        auto n = text.indexOf(separator, i);
        if (n == -1)
            break;
        t.lines.append(text.mid(i, n - i));
        i = n + separator.size();
    }

    return t;
}
}
