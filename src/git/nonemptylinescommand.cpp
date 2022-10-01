/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "nonemptylinescommand.h"

#include <QString>

namespace Git
{

const QStringList &NonEmptyLinesCommand::lines() const
{
    return mLines;
}

NonEmptyLinesCommand::NonEmptyLinesCommand()
    : ParameteresCommand()
{
}

void NonEmptyLinesCommand::parseOutput(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(errorOutput)
    const auto out = QString(output).split(QLatin1Char('\n'));

    for (const auto &line : out) {
        const auto b = line.trimmed();
        if (b.isEmpty())
            continue;

        mLines.append(b.trimmed());
    }
}

} // namespace Git
