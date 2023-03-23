/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "solution.h"

namespace Diff
{
Q_REQUIRED_RESULT Solution longestCommonSubsequence(const QStringList &source, const QStringList &target);

Q_REQUIRED_RESULT Solution3 longestCommonSubsequence(const QStringList &source, const QStringList &target, const QStringList &target2);
}
