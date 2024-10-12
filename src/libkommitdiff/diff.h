/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "lcs.h"
#include "libkommitdiff_export.h"
#include "results.h"
#include "segments.h"
#include "types.h"

#include <QStringList>

namespace Diff
{

namespace
{
int getRight(int baseLeft, const QList<LcsResult>::iterator &it)
{
    auto diff = baseLeft - it->leftStart;
    return it->rightStart + diff;
}
}

QStringList take(QStringList &list, int count);
int remove(QStringList &list, int count);

[[nodiscard]] QList<DiffSegment *> LIBKOMMITDIFF_EXPORT diff(const QString &oldText, const QString &newText);
[[nodiscard]] QList<DiffSegment *> LIBKOMMITDIFF_EXPORT diff(const QStringList &oldText, const QStringList &newText);

[[nodiscard]] Diff2Result LIBKOMMITDIFF_EXPORT diff2(const QString &oldText, const QString &newText);

[[nodiscard]] Diff3Result LIBKOMMITDIFF_EXPORT diff3(const QString &base, const QString &local, const QString &remote);
[[nodiscard]] QList<MergeSegment *> LIBKOMMITDIFF_EXPORT diff3(const QStringList &base, const QStringList &local, const QStringList &remote);

[[nodiscard]] QMap<QString, DiffType> LIBKOMMITDIFF_EXPORT diffDirs(const QString &dir1, const QString &dir2);

template<typename T>
MergeResult<T> diff3_2(const QList<T> &base, const QList<T> &local, const QList<T> &remote, const DiffOptions<T> &opts = {})
{
    std::function<bool(const T &, const T &)> equals = [&opts](const T &s1, const T &s2) -> bool {
        return opts.equals(s1, s2);
    };
    using namespace std::placeholders;
    QList<LcsResult> withLocal = longestCommonSubsequence(base, local, equals);
    QList<LcsResult> withRemote = longestCommonSubsequence(base, remote, equals);

    QList<LcsResult>::iterator itLocal = withLocal.begin();
    QList<LcsResult>::iterator itRemote = withRemote.begin();

    auto indexLocal{0};
    auto indexRemote{0};
    int indexBase{};

    QList<MergeSegment2*> result;

    while (itLocal != withLocal.end() && itRemote != withRemote.end()) {

        if (itLocal->leftStart == itRemote->leftStart) {
        } else if (itLocal->leftStart < itRemote->leftStart) {
            if (itRemote->leftStart > itLocal->leftEnd) {
                ++itLocal;
                continue;
            }
        } else if (itRemote->leftStart < itLocal->leftStart) {
            if (itLocal->leftStart > itRemote->leftEnd) {
                ++itRemote;
                continue;
            }
        }
        auto commonStart = std::max(itLocal->leftStart, itRemote->leftStart);
        auto commonEnd = std::min(itLocal->leftEnd, itRemote->leftEnd);

        auto seg = new MergeSegment2; //TODO: check memory leak
        seg->baseStart = commonStart;
        seg->baseSize = seg->localSize = seg->remoteSize = commonEnd - commonStart + 1;
        seg->localStart = indexLocal;
        seg->remoteStart = indexRemote;
        seg->type = MergeSegment2::Type::Unchanged;

        if (commonStart > indexBase || itRemote->rightStart > indexRemote || itLocal->rightStart > indexLocal) {
            auto segChanged = new MergeSegment2;
            segChanged->baseStart = indexBase;
            segChanged->baseSize = commonStart - indexBase;

            auto localEnd = getRight(commonStart - 1, itLocal);
            auto remoteEnd = getRight(commonStart - 1, itRemote);

            segChanged->localStart = indexLocal;
            segChanged->localSize = localEnd - segChanged->localStart + 1;

            segChanged->remoteStart = indexRemote;
            segChanged->remoteSize = remoteEnd - segChanged->remoteStart + 1;

            if (segChanged->remoteSize && segChanged->localSize)
                segChanged->type = MergeSegment2::Type::ChangedOnBoth;
            else if (segChanged->localSize)
                segChanged->type = MergeSegment2::Type::OnlyOnLocal;
            else if (segChanged->remoteSize)
                segChanged->type = MergeSegment2::Type::OnlyOnRemote;
            else
                segChanged->type = MergeSegment2::Type::Unchanged;

            if (Q_UNLIKELY(segChanged->type != MergeSegment2::Type::Unchanged))
                result << segChanged;
            else
                qDebug() << "Invalid segment detected";

            indexLocal += segChanged->localSize;
            indexRemote += segChanged->remoteSize;
        }

        seg->localStart = getRight(commonStart, itLocal);
        seg->remoteStart = getRight(commonStart, itRemote);
        seg->type = MergeSegment2::Type::Unchanged;

        result << seg;

        indexLocal = seg->localStart + seg->localSize;
        indexRemote = seg->remoteStart + seg->remoteSize;
        indexBase = commonEnd + 1;

        if (itLocal->leftEnd < indexBase)
            ++itLocal;

        if (itRemote->leftEnd < indexBase)
            ++itRemote;
    }

    if (itLocal != withLocal.end() || itRemote != withRemote.end()) {
        auto segChanged = new MergeSegment2;
        segChanged->baseStart = indexBase;
        segChanged->baseSize = base.size() - indexBase;

        segChanged->localStart = indexLocal + 1;
        segChanged->localSize = local.size() - indexLocal;

        segChanged->remoteStart = indexRemote + 1;
        segChanged->remoteSize = remote.size() - indexRemote;

        segChanged->type = MergeSegment2::Type::ChangedOnBoth;

        result << segChanged;
    }

    MergeResult<QString> ret;
    ret.base = base;
    ret.local = local;
    ret.remote = remote;
    ret.segments = result;
    return ret;
}

MergeResult<Text> LIBKOMMITDIFF_EXPORT diff3String(const QString &base,
                                                   const QString &local,
                                                   const QString &remote,
                                                   const DiffOptions<QString> &opts = DiffOptions<QString>{});
MergeResult<Text>
    LIBKOMMITDIFF_EXPORT diff3File(const QString &base, const QString &local, const QString &remote, const DiffOptions<QString> &opts = DiffOptions<QString>{});

} // namespace Diff
