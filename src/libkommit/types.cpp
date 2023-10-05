/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <QString>

#include "types.h"

namespace Git
{

QString convertToQString(git_buf *buf)
{
    return QString();
}

QStringList convert(git_strarray *arr)
{
    QStringList list;
    for (size_t i = 0; i < arr->count; ++i) {
        list << QString{arr->strings[i]};
    }
    return list;
}

QString convertToString(const git_oid *id, const int len)
{
    QString result = "";
    int lengthOfString = len;

    QString s;
    for (int i = 0; i < lengthOfString; i++) {
        s = QString("%1").arg(id->id[i], 0, 16);

        if (s.length() == 1)
            result.append("0");

        result.append(s);
    }

    return result;
}

void addToArray(git_strarray *arr, const QString &value)
{
    arr->strings = new char *[1];
    arr->strings[0] = value.toLocal8Bit().data();
    arr->count = 0;
}

}
