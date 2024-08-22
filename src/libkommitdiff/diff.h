/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include "results.h"
#include "segments.h"
#include "types.h"

#include <QStringList>

namespace Diff
{

QStringList take(QStringList &list, int count);
int remove(QStringList &list, int count);

[[nodiscard]] QList<DiffSegment *> LIBKOMMITDIFF_EXPORT diff(const QString &oldText, const QString &newText);
[[nodiscard]] QList<DiffSegment *> LIBKOMMITDIFF_EXPORT diff(const QStringList &oldText, const QStringList &newText);

[[nodiscard]] Diff2Result LIBKOMMITDIFF_EXPORT diff2(const QString &oldText, const QString &newText);

[[nodiscard]] Diff3Result LIBKOMMITDIFF_EXPORT diff3(const QString &base, const QString &local, const QString &remote);
[[nodiscard]] QList<MergeSegment *> LIBKOMMITDIFF_EXPORT diff3(const QStringList &base, const QStringList &local, const QStringList &remote);

[[nodiscard]] QMap<QString, DiffType> LIBKOMMITDIFF_EXPORT diffDirs(const QString &dir1, const QString &dir2);

} // namespace Diff
