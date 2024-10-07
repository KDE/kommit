/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitdiff_export.h"
#include <QByteArray>
#include <QFile>
#include <QString>
#include <QStringView>
#include <cstddef>

namespace Diff
{

enum class LineEnding { None, Cr, Lf, CrLf };

enum class DiffType { Unchanged, Added, Removed, Modified };

// std::size_t qHash(const DiffType &type);

enum MergeType { None, KeepLocal, KeepRemote, KeepLocalThenRemote, KeepRemoteThenLocal, MergeCustom };

enum class SegmentType {
    SameOnBoth,
    OnlyOnLeft,
    OnlyOnRight,
    DifferentOnBoth,
};

enum MergeDiffType { Unchanged, LocalAdd, RemoteAdd, BothChanged };

template<typename T>
struct DiffOptions {
    bool equals(const T &n1, const T &n2) const
    {
        return n1 == n2;
    }
};

template<>
struct DiffOptions<QString> {
    bool ignoreCase{false};
    bool ignoreWhiteSpaces{false};

    bool equals(const QString &s1, const QString &s2) const
    {
        if (ignoreWhiteSpaces)
            return QString::compare(s1.trimmed(), s2.trimmed(), ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;

        return QString::compare(s1, s2, ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;
    }
};

template<>
struct DiffOptions<QByteArray> {
    bool ignoreCase{false};
    bool ignoreWhiteSpaces{false};

    bool equals(const QByteArray &s1, const QByteArray &s2) const
    {
        if (ignoreWhiteSpaces)
            return s1.trimmed().compare(s2.trimmed(), ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;

        return s1.compare(s2, ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;
    }
};

template<>
struct DiffOptions<QStringView> {
    bool ignoreCase{false};
    bool ignoreWhiteSpaces{false};

    bool equals(const QStringView &s1, const QStringView &s2) const
    {
        if (ignoreWhiteSpaces)
            return s1.trimmed().compare(s2.trimmed(), ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;

        return s1.compare(s2, ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;
    }
};

template<>
struct DiffOptions<QFile> {
    bool ignoreCase{false};
    bool ignoreWhiteSpaces{false};
    bool checkTime;

    bool equals(const QString &s1, const QString &s2) const
    {
        if (ignoreWhiteSpaces)
            return QString::compare(s1.trimmed(), s2.trimmed(), ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;

        return QString::compare(s1, s2, ignoreCase ? Qt::CaseInsensitive : Qt::CaseSensitive) == 0;
    }
};
}
