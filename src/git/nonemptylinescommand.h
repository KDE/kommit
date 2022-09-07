/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NONEMPTYLINESCOMMAND_H
#define NONEMPTYLINESCOMMAND_H

#include "parameterescommand.h"

namespace Git {

class NonEmptyLinesCommand : public ParameteresCommand
{
    QStringList _lines;

public:
    NonEmptyLinesCommand();

    void parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;
    const QStringList &lines() const;
};

} // namespace Git

#endif // NONEMPTYLINESCOMMAND_H
