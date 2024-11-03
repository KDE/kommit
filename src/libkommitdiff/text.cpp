/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "text.h"

#include <QDebug>

namespace Diff
{

namespace Impl
{
LineEnding detectLineEnding(const QString &text, QString *separator)
{
    LineEnding le{LineEnding::None};
    for (const auto &ch : text) {
        if (le == LineEnding::Cr) {
            if (ch == QLatin1Char('\n')) {
                if (separator)
                    *separator = QStringLiteral("\r\n");
                le = LineEnding::CrLf;
                break;
            }
        }
        if (ch == QLatin1Char('\r')) {
            if (separator)
                *separator = QStringLiteral("\r");
            le = LineEnding::Cr;
            break;
        }
        if (ch == QLatin1Char('\n')) {
            if (separator)
                *separator = QStringLiteral("\n");
            le = LineEnding::Lf;
            break;
        }
    }

    return le;
}

}

Text::Text()
    : lineEnding(LineEnding::None)
{
}

Text readLines(const QString &text)
{
    if (text.isEmpty())
        return {};

    QString separator;
    LineEnding le = Impl::detectLineEnding(text, &separator);

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
        if (n == -1) {
            t.lines.append(text.mid(i));
            break;
        }
        t.lines.append(text.mid(i, n - i));
        i = n + separator.size();
    }

    return t;
}

TextView::TextView(const QString &text)
    : lineEnding{LineEnding::None}
{
    if (text.isEmpty())
        return;

    QString separator;
    LineEnding le = Impl::detectLineEnding(text, &separator);

    if (le == LineEnding::None) {
        qWarning() << "Unable to detect line ending";
        return;
    }

    int i{0};
    content = text;
    lineEnding = le;
    while (i != -1) {
        auto n = text.indexOf(separator, i);
        if (n == -1) {
            lines << QStringView{text}.mid(i);
            break;
        }
        lines << QStringView{text}.mid(i, n - i);
        i = n + separator.size();
    }
}
}
