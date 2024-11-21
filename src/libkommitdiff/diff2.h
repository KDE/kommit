/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QList>
#include <QSharedPointer>

#include "hunk.h"
#include "lcs.h"
#include "results.h"
#include "segments.h"
#include "solution.h"

namespace Diff
{

namespace Impl
{
template<typename T>
QList<DiffHunk *> diff(const QList<T> &oldText, const QList<T> &newText, const DiffOptions<T> &opts = {})
{
    if (oldText == newText) {
        auto segment = new DiffHunk;
        segment->type = SegmentType::SameOnBoth;
        // segment->oldText = oldText;
        // segment->newText = newText;

        segment->left.size = oldText.size();
        segment->right.size = newText.size();
        return {segment};
    } else if (newText.isEmpty()) {
        auto segment = new DiffHunk;
        segment->type = SegmentType::OnlyOnLeft;
        // segment->oldText = oldText;
        // segment->newText = newText;
        segment->left.size = oldText.size();
        segment->right.size = 0;
        return {segment};
    } else if (oldText.isEmpty()) {
        auto segment = new DiffHunk;
        segment->type = SegmentType::OnlyOnRight;
        // segment->oldText = oldText;
        // segment->newText = newText;
        segment->left.size = 0;
        segment->right.size = newText.size();
        return {segment};
    }

    std::function<bool(const T &, const T &)> equals = [&opts](const T &s1, const T &s2) -> bool {
        return opts.equals(s1, s2);
    };

    QList<LcsResult> lcs = longestCommonSubsequence<T>(oldText, newText, equals);

    QList<DiffHunk *> ret;

    int leftIndex{0};
    int rightIndex{0};
    auto add = [&ret, &oldText, &newText](int leftStart, int leftEnd, int rightStart, int rightEnd, bool same) {
        Q_ASSERT(leftStart <= leftEnd);
        Q_ASSERT(rightStart <= rightEnd);
        auto s = new DiffHunk;
        s->left.begin = leftStart;
        s->left.size = leftEnd - leftStart;
        s->right.begin = rightStart;
        s->right.size = rightEnd - rightStart;

        // s->oldText = oldText.mid(leftStart, leftEnd - leftStart);
        // s->newText = newText.mid(rightStart, rightEnd - rightStart);
        if (same) {
            s->type = SegmentType::SameOnBoth;
        } else {
            if (leftStart == leftEnd)
                s->type = SegmentType::OnlyOnRight;
            else if (rightStart == rightEnd)
                s->type = SegmentType::OnlyOnLeft;
            else
                s->type = SegmentType::DifferentOnBoth;
        }
        ret << s;
        // qDebug().noquote().nospace() << "Segment: " << s->oldLineStart << "->" << s->oldLineEnd << "    <>    " << s->newLineStart << "->" << s->newLineEnd;
    };

    for (auto const &p : lcs) {
        // Note: lcs returns ranges like [begin, end],
        // but we need ranges like [begin, end), so add
        // 1 to p.leftEnd and p.rightEnd

        // Blocks with diff
        if (p.leftStart != leftIndex || p.rightStart != rightIndex)
            add(leftIndex, p.leftStart, rightIndex, p.rightStart, false);

        leftIndex = p.leftEnd + 1;
        rightIndex = p.rightEnd + 1;

        // Blocks with no diff
        add(p.leftStart, leftIndex, p.rightStart, rightIndex, true);
    }

    // Any remaining block?
    if (leftIndex < oldText.size() || rightIndex < newText.size())
        add(leftIndex, oldText.size(), rightIndex, newText.size(), false);

    return ret;
}
}

class Diff2TextResultPrivate;
struct LIBKOMMITDIFF_EXPORT Diff2TextResult {
    const Text &left() const;
    const Text &right() const;
    const QList<DiffHunk *> &hunks() const;

    Diff2TextResult();
    Diff2TextResult(Text left, Text right, QList<DiffHunk *> hunks);

    Diff2TextResult(const Diff2TextResult &other);
    Diff2TextResult &operator=(const Diff2TextResult &other);

private:
    QSharedPointer<Diff2TextResultPrivate> d;
};

struct LIBKOMMITDIFF_EXPORT Diff2TextViewResult {
    const TextView left;
    const TextView right;
    const QList<DiffSegment *> segments;
    Diff2TextViewResult(Text left, Text right, QList<DiffSegment *> segments);
    ~Diff2TextViewResult();
};

[[nodiscard]] Diff2TextResult LIBKOMMITDIFF_EXPORT diff2(const QString &oldText, const QString &newText, const DiffOptions<QString> &opts = {});
[[nodiscard]] QList<DiffHunk *> LIBKOMMITDIFF_EXPORT diff2(const QStringList &oldText, const QStringList &newText, const DiffOptions<QString> &opts = {});
}
