/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include "commands/abstractcommand.h"

#include <QStringList>

namespace Git
{
void run(const QString &workingDir, const QStringList &args);
void run(const QString &workingDir, AbstractCommand &cmd);

LIBGITKLIENT_EXPORT QByteArray runGit(const QString &workingDir, const QStringList &args);
LIBGITKLIENT_EXPORT QStringList readAllNonEmptyOutput(const QString &workingDir, const QStringList &cmd, bool trim = true);
} // namespace Git
