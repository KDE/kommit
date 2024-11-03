/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "array.h"
#include "libkommitdiff_export.h"
#include "solution.h"

#include <QBitArray>

namespace Diff
{

template<typename T>
struct LcsOptions {
    bool equals(T n1, T n2)
    {
        return n1 == n2;
    }
};

template<>
struct LcsOptions<QString> {
    bool ignoreCase{false};
    bool ignoreWhiteSpaces{false};

    bool equals(const QString &s1, const QString &s2)
    {
        if (ignoreWhiteSpaces)
            return QString::compare(s1.trimmed(), s2.trimmed(), ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;

        return QString::compare(s1, s2, ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;
    }
};

struct LcsResult {
    int leftStart;
    int leftEnd;
    int rightStart;
    int rightEnd;
};

int LIBKOMMITDIFF_EXPORT maxIn(int first, int second, int third);
int LIBKOMMITDIFF_EXPORT maxIn(int first, int second);
int LIBKOMMITDIFF_EXPORT maxIn(const QList<int> &list);

template<typename T>
[[nodiscard]] QList<LcsResult> longestCommonSubsequence(const QList<T> &left, const QList<T> &right, const std::function<bool(const T &, const T &)> &equals)
{
    Array2<int> l(left.size() + 1, right.size() + 1);

    for (int i = 0; i <= left.count(); i++) {
        for (int j = 0; j <= right.count(); j++) {
            if (i == 0 || j == 0) {
                l(i, j) = 0;
            } else if (equals(left.at(i - 1), right.at(j - 1))) {
                l(i, j) = l(i - 1, j - 1) + 1;
            } else {
                l(i, j) = qMax(l(i - 1, j), l(i, j - 1));
            }
        }
    }

    int i = left.count();
    int j = right.count();
    QList<LcsResult> result;

    while (i > 0 && j > 0) {
        if (equals(left.at(i - 1), right.at(j - 1))) {
            int leftEnd = i - 1;
            int rightEnd = j - 1;
            int leftStart = leftEnd;
            int rightStart = rightEnd;

            // Move diagonally while elements match
            while (i > 0 && j > 0 && equals(left.at(i - 1), right.at(j - 1))) {
                --i;
                --j;
                leftStart = i;
                rightStart = j;
            }

            // Add the matched subsequence to result
            result.prepend({leftStart, leftEnd, rightStart, rightEnd});

            // i--;
            // j--;
            // if (!started) {
            //     si = i;
            //     sj = j;
            //     started = true;
            // }
        } else {
            // if (started) {
            //     r << LcsResult{si, i, sj, j};
            //     started = false;
            // }

            int n = maxIn(l(i - 1, j), l(i, j - 1), l(i - 1, j - 1));
            switch (n) {
            case 1:
                i--;
                break;
            case 2:
                j--;
                break;
            default:
                i--;
                j--;
                break;
            }
        }
    }

    return result;
}

Q_DECL_DEPRECATED
[[nodiscard]] Solution longestCommonSubsequence(const QStringList &source, const QStringList &target);

Q_DECL_DEPRECATED
[[nodiscard]] Solution3 longestCommonSubsequence(const QStringList &source, const QStringList &target, const QStringList &target2);
}
