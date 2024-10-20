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
#include "libkommit_global.h"

namespace Git
{

class Branch;
class Commit;
class Tag;
class Remote;

// #define DECLARE_TYPE(x)                                                                                                                                        \
//     using x##Pointer = Pointer<Git::x>;                                                                                                                        \
//     using x##List = PointerList<Git::x>;

// DECLARE_TYPE(Commit)
// DECLARE_TYPE(Tag)
// DECLARE_TYPE(Branch)
// DECLARE_TYPE(Remote)

LIBKOMMIT_EXPORT inline const char *toChar(const QString &s);

QString convertToQString(git_buf *buf);
QStringList convert(git_strarray *arr);
QString convertToString(const git_oid *id, const int len);
void addToArray(git_strarray *arr, const QString &value);

#define toConstChars(s) s.toLocal8Bit().constData()
}
