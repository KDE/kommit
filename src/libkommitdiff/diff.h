/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "diff2.h"
#include "diff3.h"

#include "libkommitdiff_export.h"

#include <QMap>

namespace Diff
{

QMap<QString, DiffType> LIBKOMMITDIFF_EXPORT diffDirs(const QString &dir1, const QString &dir2);

}
