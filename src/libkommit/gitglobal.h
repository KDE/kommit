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

LIBKOMMIT_EXPORT QIcon statusIcon(ChangeStatus status);

LIBKOMMIT_EXPORT QByteArray runGit(const QString &workingDir, const QStringList &args);
LIBKOMMIT_EXPORT QStringList readAllNonEmptyOutput(const QString &workingDir, const QStringList &cmd, bool trim = true);

#define BEGIN int err = 0;
#define STEP err = err ? err:
#define END                                                                                                                                                    \
    do {                                                                                                                                                       \
        if (err) {                                                                                                                                             \
            auto __git_err = git_error_last();                                                                                                                 \
            auto __git_err_class = __git_err->klass;                                                                                                           \
            auto __git_err_msg = QString{__git_err->message};                                                                                                  \
            qDebug().noquote().nospace() << "libgit2 error: " << err << ", class: " << __git_err_class << ", Mssage: " << __git_err_msg;                       \
        }                                                                                                                                                      \
    } while (false)

#define RETURN_COND(ok, error)                                                                                                                                 \
    do {                                                                                                                                                       \
        return err ? error : ok;                                                                                                                               \
    } while (false)

#define THROW                                                                                                                                                  \
    do {                                                                                                                                                       \
        if (!err)                                                                                                                                              \
            throw new Exception{err, gitErrorMessage(err)};                                                                                                    \
    } while (false)

#define RETURN_IF_ERR(value)                                                                                                                                   \
    do {                                                                                                                                                       \
        if (err)                                                                                                                                               \
            return value;                                                                                                                                      \
    } while (false)

#define RETURN_IF_ERR_BOOL                                                                                                                                     \
    do {                                                                                                                                                       \
        if (err)                                                                                                                                               \
            return false;                                                                                                                                      \
    } while (false)

#define RETURN_IF_ERR_VOID                                                                                                                                     \
    do {                                                                                                                                                       \
        if (err)                                                                                                                                               \
            return;                                                                                                                                            \
    } while (false)

} // namespace Git
