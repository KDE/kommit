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

template<class T>
using PointerList = QList<QSharedPointer<T>>;

LIBKOMMIT_EXPORT inline const char *toChar(const QString &s);

QString convertToQString(git_buf *buf);
QStringList convert(git_strarray *arr);
QString convertToString(const git_oid *id, const int len);
void addToArray(git_strarray *arr, const QString &value);

QSharedPointer<git_tree> makeShared(git_tree *tree);

#define toConstChars(s) s.toLocal8Bit().constData()
}
