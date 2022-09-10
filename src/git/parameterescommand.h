/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "commands/abstractcommand.h"

namespace Git {

class ParameteresCommand : public AbstractCommand
{
    QStringList _args;

public:
    ParameteresCommand();
    const QStringList &args() const;
    void setArgs(const QStringList &newArgs);
    void appendArg(const QString &arg);

    QStringList generateArgs() const override;
};

} // namespace Git

