/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "text.h"

#include <QDebug>

namespace Diff
{
Text readLines(const QString &text)
{
    if (text.isEmpty())
        return {};

    LineEnding le{LineEnding::None};
    QString seprator;
    for (const auto &ch : text) {
        if (le == LineEnding::Cr) {
            if (ch == QLatin1Char('\n')) {
                le = LineEnding::CrLf;
                seprator = QStringLiteral("\r\n");
            }
            break;
        }
        if (ch == QLatin1Char('\r')) {
            le = LineEnding::Cr;
            seprator = QStringLiteral("\r");
            continue;
        }
        if (ch == QLatin1Char('\n')) {
            seprator = QStringLiteral("\n");
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
    t.lineEnding = le;
    while (i != -1) {
        auto n = text.indexOf(seprator, i);
        if (n == -1)
            break;
        t.lines.append(text.mid(i, n - i));
        i = n + seprator.size();
    }

    return t;
}

}
