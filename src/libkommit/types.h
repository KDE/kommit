/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QSharedPointer>
#include <QString>

#include <git2/buffer.h>
#include <git2/strarray.h>
#include <git2/types.h>

#include "libkommit_export.h"

namespace Git
{

class Buf : public QSharedPointer<git_buf>
{
public:
    Buf()
        : QSharedPointer<git_buf>{nullptr, git_buf_dispose}
    {
    }
};

LIBKOMMIT_EXPORT inline const char *toChar(const QString &s);

QString convertToQString(git_buf *buf);
QStringList convert(git_strarray *arr);
QString convertToString(const git_oid *id, const int len);
void addToArray(git_strarray *arr, const QString &value);

QSharedPointer<git_tree> makeShared(git_tree *tree);

#define BEGIN int err = 0;
#define STEP err = err ? err:
#define PRINT_ERROR                                                                                                                                            \
    do {                                                                                                                                                       \
        if (err) {                                                                                                                                             \
            const_cast<Manager *>(this)->checkError(err);                                                                                                      \
            qDebug() << "Error" << Q_FUNC_INFO << err << ":" << gitErrorMessage(err);                                                                          \
        }                                                                                                                                                      \
    } while (false)

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

#define toConstChars(s) s.toLocal8Bit().constData()
}
