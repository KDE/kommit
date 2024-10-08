/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "commands/abstractcommand.h"
#include "libkommit_export.h"

#include <QStringList>

namespace Git
{

LIBKOMMIT_EXPORT void run(const QString &workingDir, const QStringList &args);
LIBKOMMIT_EXPORT void run(const QString &workingDir, const AbstractCommand &cmd);

[[nodiscard]] LIBKOMMIT_EXPORT QString runGit(const QString &workingDir, const QStringList &args);
[[nodiscard]] LIBKOMMIT_EXPORT QStringList readAllNonEmptyOutput(const QString &workingDir, const QStringList &cmd, bool trim = true);

} // namespace Git
