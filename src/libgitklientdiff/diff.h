/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libgitklientdiff_export.h"
#include "results.h"
#include "segments.h"
#include "types.h"

#include <QStringList>

namespace Diff
{

int matchesCount(const QStringList &base, const QStringList &local, const QStringList &remote);
QStringList take(QStringList &list, int count);
int remove(QStringList &list, int count);

QList<DiffSegment *> LIBGITKLIENTDIFF_EXPORT diff(const QString &oldText, const QString &newText);
QList<DiffSegment *> LIBGITKLIENTDIFF_EXPORT diff(const QStringList &oldText, const QStringList &newText);

Diff2Result LIBGITKLIENTDIFF_EXPORT diff2(const QString &oldText, const QString &newText);

Diff3Result LIBGITKLIENTDIFF_EXPORT diff3(const QString &base, const QString &local, const QString &remote);
QList<MergeSegment *> LIBGITKLIENTDIFF_EXPORT diff3(const QStringList &base, const QStringList &local, const QStringList &remote);

QMap<QString, DiffType> LIBGITKLIENTDIFF_EXPORT diffDirs(const QString &dir1, const QString &dir2);

} // namespace Diff
