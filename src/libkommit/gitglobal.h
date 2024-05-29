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
enum class ChangeStatus {
    Unknown,
    Unmodified,
    Modified,
    Added,
    Removed,
    Renamed,
    Copied,
    UpdatedButInmerged,
    Ignored,
    Untracked,
    TypeChange,
    Unreadable,
    Conflicted,
};

LIBKOMMIT_EXPORT void run(const QString &workingDir, const QStringList &args);
LIBKOMMIT_EXPORT void run(const QString &workingDir, const AbstractCommand &cmd);

Q_REQUIRED_RESULT LIBKOMMIT_EXPORT QIcon statusIcon(ChangeStatus status);

Q_REQUIRED_RESULT LIBKOMMIT_EXPORT QByteArray runGit(const QString &workingDir, const QStringList &args);
Q_REQUIRED_RESULT LIBKOMMIT_EXPORT QStringList readAllNonEmptyOutput(const QString &workingDir, const QStringList &cmd, bool trim = true);

} // namespace Git
