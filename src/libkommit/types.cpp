/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <QStringList>
#include <git2/tree.h>

#include "types.h"

namespace Git
{

namespace Deleters
{

void deleteGitTree(git_tree *tree)
{
    git_tree_free(tree);
}

}

QString convertToQString(git_buf *buf)
{
    return QString(buf->ptr);
}

QStringList convert(git_strarray *arr)
{
    QStringList list;
    list.reserve(arr->count);
    for (size_t i = 0; i < arr->count; ++i) {
        list << QString{arr->strings[i]};
    }
    return list;
}

QString convertToString(const git_oid *id, const int len)
{
    QString result;
    QString s;
    for (int i = 0; i < len; i++) {
        s = QStringLiteral("%1").arg(id->id[i], 0, 16);

        if (s.length() == 1)
            result.append(QLatin1Char('0'));

        result.append(s);
    }

    return result;
}

void addToArray(git_strarray *arr, const QString &value)
{
    arr->strings = new char *[1];
    arr->strings[0] = value.toLocal8Bit().data();
    arr->count = 1;
}

void addToArray(git_strarray *arr, const QStringList &list)
{
    arr->strings = new char *[list.size()];
    for (int i = 0; i < list.size(); ++i) {
        arr->strings[i] = list.at(i).toLocal8Bit().data();
    }
    arr->count = 1;
}
}
