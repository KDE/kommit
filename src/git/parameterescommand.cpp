/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "parameterescommand.h"

namespace Git
{

ParameteresCommand::ParameteresCommand()
    : AbstractCommand()
{
}

const QStringList &ParameteresCommand::args() const
{
    return mArgs;
}

void ParameteresCommand::setArgs(const QStringList &newArgs)
{
    mArgs = newArgs;
}

void ParameteresCommand::appendArg(const QString &arg)
{
    mArgs.append(arg);
}

QStringList ParameteresCommand::generateArgs() const
{
    return mArgs;
}

} // namespace Git
