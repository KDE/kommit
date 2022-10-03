/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "parameterescommand.h"

namespace Git
{

class NonEmptyLinesCommand : public ParameteresCommand
{
public:
    NonEmptyLinesCommand();

    void parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;
    const QStringList &lines() const;

private:
    QStringList mLines;
};

} // namespace Git
