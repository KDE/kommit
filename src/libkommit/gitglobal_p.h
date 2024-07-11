/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#define LIBGIT_ERR_CHECK_VARIABLE __err

#define BEGIN int LIBGIT_ERR_CHECK_VARIABLE = 0;
#define RESTART LIBGIT_ERR_CHECK_VARIABLE = 0;
#define STEP LIBGIT_ERR_CHECK_VARIABLE = LIBGIT_ERR_CHECK_VARIABLE ? LIBGIT_ERR_CHECK_VARIABLE:

#ifdef QDEBUG_H
#define END                                                                                                                                                    \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE) {                                                                                                                       \
            auto __git_err = git_error_last();                                                                                                                 \
            auto __git_err_class = __git_err->klass;                                                                                                           \
            auto __git_err_msg = QString{__git_err->message};                                                                                                  \
            qDebug().noquote().nospace() << "libgit2 error: " << LIBGIT_ERR_CHECK_VARIABLE << ", class: " << __git_err_class                                   \
                                         << ", Message: " << __git_err_msg;                                                                                    \
        }                                                                                                                                                      \
    } while (false)
#else
#define END
#endif

#define PRINT_ERROR END
#define RETURN_COND(ok, error)                                                                                                                                 \
    do {                                                                                                                                                       \
        return LIBGIT_ERR_CHECK_VARIABLE ? error : ok;                                                                                                         \
    } while (false)

#define RETURN_IF_ERR(value)                                                                                                                                   \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE)                                                                                                                         \
            return value;                                                                                                                                      \
    } while (false)

#define RETURN_IF_ERR_BOOL                                                                                                                                     \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE)                                                                                                                         \
            return false;                                                                                                                                      \
    } while (false)

#define RETURN_IF_ERR_VOID                                                                                                                                     \
    do {                                                                                                                                                       \
        if (LIBGIT_ERR_CHECK_VARIABLE)                                                                                                                         \
            return;                                                                                                                                            \
    } while (false)

#define IS_OK !LIBGIT_ERR_CHECK_VARIABLE
#define IS_ERROR LIBGIT_ERR_CHECK_VARIABLE
