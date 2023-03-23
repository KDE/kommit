/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "lcs.h"

#include "array.h"

namespace Diff
{

template<typename T>
inline bool isEqual(const T &i1, const T &i2)
{
    return i1 == i2;
}
template<>
inline bool isEqual<QString>(const QString &i1, const QString &i2)
{
    return i1.trimmed() == i2.trimmed();
}

int maxIn(int first, int second, int third)
{
    if (first == second && second == third)
        return 0;

    if (third > first && third > second)
        return 3;

    if (first > second && first > third)
        return 1;

    if (second > first && second > third)
        return 2;

    return 4;
}

int maxIn(int first, int second)
{
    if (first > second)
        return 1;

    if (second > first)
        return 2;

    return 0;
}

int maxIn(const QList<int> &list)
{
    if (list.empty())
        return -1;
    auto i = std::max_element(list.begin(), list.end());
    return std::distance(list.begin(), i);
    //    int max{list.first()};
    //    int maxIndex{0};
    //    int index{0};
    //    for (const auto &i : list) {
    //        if (i > max) {
    //            max = i;
    //            maxIndex = index;
    //        }
    //        index++;
    //    }

    //    return maxIndex;
}

Solution longestCommonSubsequence(const QStringList &source, const QStringList &target)
{
    Array2<int> l(source.size() + 1, target.size() + 1);

    for (int i = 0; i <= source.count(); i++) {
        for (int j = 0; j <= target.count(); j++) {
            if (i == 0 || j == 0) {
                l(i, j) = 0;
            } else if (isEqual(source.at(i - 1), target.at(j - 1))) {
                l(i, j) = l(i - 1, j - 1) + 1;
            } else {
                l(i, j) = qMax(l(i - 1, j), l(i, j - 1));
            }
        }
    }

    int i = source.count();
    int j = target.count();
    Solution r;

    while (i > 0 && j > 0) {
        if (isEqual(source.at(i - 1), target.at(j - 1))) {
            r.prepend(qMakePair(i - 1, j - 1));
            i--;
            j--;
        } else {
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

    return r;
}

Solution3 longestCommonSubsequence(const QStringList &source, const QStringList &target, const QStringList &target2)
{
    Array3<int> l(source.size() + 1, target.size() + 1, target2.size() + 1);

    for (int i = 0; i <= source.count(); i++)
        for (int j = 0; j <= target.count(); j++)
            for (int k = 0; k <= target2.count(); ++k)
                if (i == 0 || j == 0 || k == 0) {
                    l(i, j, k) = 0;
                } else if (source.at(i - 1) == target.at(j - 1) && source.at(i - 1) == target2.at(k - 1)) {
                    l(i, j, k) = l(i - 1, j - 1, k - 1) + 1;
                } else {
                    l(i, j, k) = std::max(std::max(l(i - 1, j, k), l(i, j - 1, k)), l(i, j, k - 1));
                }

    int i = source.count();
    int j = target.count();
    int k = target2.count();

    Solution3 r;
    while (i > 0 && j > 0 && k > 0) {
        if (source.at(i - 1) == target.at(j - 1) && source.at(i - 1) == target2.at(k - 1)) {
            //            longestCommonSubsequence[index - 1] = source.at(i - 1);
            r.prepend({i - 1, j - 1, k - 1});
            i--;
            j--;
            k--;
        } else {
            QList<int> list = {l(i - 1, j, k),
                               l(i, j - 1, k),
                               l(i, j, k - 1),

                               l(i - 1, j - 1, k),
                               l(i, j - 1, k - 1),
                               l(i - 1, j, k - 1),

                               l(i - 1, j - 1, k - 1)};
            int n = maxIn(list);

            switch (n) {
            case 0:
                i--;
                break;
            case 1:
                j--;
                break;
            case 2:
                k--;
                break;

            case 3:
                i--;
                j--;
                break;
            case 4:
                j--;
                k--;
                break;
            case 5:
                i--;
                k--;
                break;

            default: // also for 6
                i--;
                j--;
                k--;
                break;
            }
        }
    }
    return r;
}

}
