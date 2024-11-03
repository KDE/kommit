/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diff3.h"

#include "libkommitdiff_export.h"

namespace Diff
{

int getRight(int baseLeft, const QList<LcsResult>::iterator &it)
{
    auto diff = baseLeft - it->leftStart;
    return it->rightStart + diff;
}

MergeResult<Text> diff3String(const QString &base, const QString &local, const QString &remote, const DiffOptions<QString> &opts)
{
    MergeResult<Text> result;

    result.base = readLines(base);
    result.local = readLines(local);
    result.remote = readLines(remote);

    auto r = diff3(result.base.lines, result.local.lines, result.remote.lines, opts);
    result.segments = r.segments;
    return result;
}

}
