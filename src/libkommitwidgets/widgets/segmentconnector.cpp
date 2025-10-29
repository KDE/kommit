/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "segmentconnector.h"
#include "widgets/codeeditor.h"

#include <QPainter>
#include <QPainterPath>
#include <kommitwidgetsglobaloptions.h>

#include <KommitDiff/Diff>

SegmentConnector::SegmentConnector(QWidget *parent)
    : QWidget(parent)
{
}

SegmentConnector::~SegmentConnector()
{
    // qDeleteAll(mSegments);
}

const QList<Diff::DiffHunk *> &SegmentConnector::segments() const
{
    return mSegments;
}

void SegmentConnector::setSegments(const QList<Diff::DiffHunk *> &newSegments)
{
    mSegments = newSegments;

    int oldIndex{0};
    int newIndex{0};
    mSegmentPos.clear();

    for (auto &s : std::as_const(mSegments)) {
        if (mSameSize) {
            auto sizeMax = qMax(s->left.size, s->right.size);

            SegmentPos pos{oldIndex, static_cast<int>(oldIndex + s->left.size - 1), newIndex, static_cast<int>(newIndex + s->right.size - 1)};

            //            if (s->oldText.isEmpty())
            //                pos.leftEnd = -1;
            //            if (s->newText.isEmpty())
            //                pos.rightEnd = -1;
            mSegmentPos.insert(s, pos);

            oldIndex += sizeMax;
            newIndex += sizeMax;
        } else {
            SegmentPos pos{oldIndex, static_cast<int>(oldIndex + s->left.size - 1), newIndex, static_cast<int>(newIndex + s->right.size - 1)};

            if (!s->left.size)
                pos.leftEnd = -1;
            if (!s->right.size)
                pos.rightEnd = -1;
            mSegmentPos.insert(s, pos);

            oldIndex += s->left.size;
            newIndex += s->right.size;
        }
    }
    Q_EMIT segmentsChanged();
}

Diff::Segment *SegmentConnector::currentSegment() const
{
    return mCurrentSegment;
}

void SegmentConnector::setCurrentSegment(Diff::Segment *newCurrentSegment)
{
    mCurrentSegment = newCurrentSegment;
    update();
}

bool SegmentConnector::sameSize() const
{
    return mSameSize;
}

void SegmentConnector::setSameSize(bool newSameSize)
{
    mSameSize = newSameSize;
    Q_EMIT sameSizeChanged();
}

CodeEditor *SegmentConnector::left() const
{
    return mLeft;
}

void SegmentConnector::setLeft(CodeEditor *newLeft)
{
    mLeft = newLeft;
}

CodeEditor *SegmentConnector::right() const
{
    return mRight;
}

void SegmentConnector::setRight(CodeEditor *newRight)
{
    mRight = newRight;
}

void SegmentConnector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!mLeft || !mRight)
        return;

    QPainter painter(this);

    if (mTopMargin)
        painter.translate(QPoint(0, mTopMargin));

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), palette().base());

    for (auto s = mSegmentPos.begin(); s != mSegmentPos.end(); ++s) {
        if (s.key()->type == Diff::SegmentType::SameOnBoth)
            continue;
        const auto leftArea = mLeft->blockArea(s->leftStart, s->leftEnd);
        const auto rightArea = mRight->blockArea(s->rightStart, s->rightEnd);

        //        if (s == _currentSegment)
        //            painter.setBrush(Qt::yellow);
        //        else
        switch (s.key()->type) {
        case Diff::SegmentType::SameOnBoth:
            //            painter.setBrush(Qt::magenta);
            continue;
        case Diff::SegmentType::OnlyOnRight:
            painter.setBrush(KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Added));
            break;
        case Diff::SegmentType::OnlyOnLeft:
            painter.setBrush(KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Removed));
            break;
        case Diff::SegmentType::DifferentOnBoth:
            painter.setBrush(KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Modified));
            break;
        }

        QPainterPath poly;
        poly.moveTo(0, leftArea.first);

        poly.cubicTo(QPointF(30, leftArea.first), QPointF(width() - 30, rightArea.first), QPointF(width() - 1, rightArea.first));
        poly.lineTo(width() - 1, rightArea.second);

        poly.cubicTo(QPoint(width() - 30, rightArea.second), QPoint(30, leftArea.second), QPoint(0, leftArea.second));
        painter.setPen(Qt::NoPen);
        painter.drawPath(poly);
    }
}

int SegmentConnector::topMargin() const
{
    return mTopMargin;
}

void SegmentConnector::setTopMargin(int newTopMargin)
{
    mTopMargin = newTopMargin;
    update();
}

#include "moc_segmentconnector.cpp"
