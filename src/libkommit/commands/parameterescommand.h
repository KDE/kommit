/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "commands/abstractcommand.h"

namespace Git
{

class ParameteresCommand : public AbstractCommand
{
public:
    ParameteresCommand();
    Q_REQUIRED_RESULT const QStringList &args() const;
    void setArgs(const QStringList &newArgs);
    void appendArg(const QString &arg);

    Q_REQUIRED_RESULT QStringList generateArgs() const override;

private:
    QStringList mArgs;
};

} // namespace Git
