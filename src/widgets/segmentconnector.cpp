/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "segmentconnector.h"
#include "GitKlientSettings.h"
#include "widgets/codeeditor.h"

#include <QPainter>
#include <QPainterPath>

const QList<Diff::Segment *> &SegmentConnector::segments() const
{
    return mSegments;
}

void SegmentConnector::setSegments(const QList<Diff::Segment *> &newSegments)
{
    mSegments = newSegments;

    int oldIndex{0};
    int newIndex{0};
    mSegmentPos.clear();

    for (auto &s : mSegments) {
        if (m_sameSize) {
            auto sizeMax = qMax(s->oldText.size(), s->newText.size());

            SegmentPos pos{oldIndex, oldIndex + s->oldText.size() - 1, newIndex, newIndex + s->newText.size() - 1};

            //            if (s->oldText.isEmpty())
            //                pos.leftEnd = -1;
            //            if (s->newText.isEmpty())
            //                pos.rightEnd = -1;
            mSegmentPos.insert(s, pos);

            oldIndex += sizeMax;
            newIndex += sizeMax;
        } else {
            SegmentPos pos{oldIndex, oldIndex + s->oldText.size() - 1, newIndex, newIndex + s->newText.size() - 1};

            if (s->oldText.isEmpty())
                pos.leftEnd = -1;
            if (s->newText.isEmpty())
                pos.rightEnd = -1;
            mSegmentPos.insert(s, pos);

            oldIndex += s->oldText.size();
            newIndex += s->newText.size();
        }
    }
}

Diff::Segment *SegmentConnector::currentSegment() const
{
    return _currentSegment;
}

void SegmentConnector::setCurrentSegment(Diff::Segment *newCurrentSegment)
{
    _currentSegment = newCurrentSegment;
    update();
}

bool SegmentConnector::sameSize() const
{
    return m_sameSize;
}

void SegmentConnector::setSameSize(bool newSameSize)
{
    m_sameSize = newSameSize;
}

SegmentConnector::SegmentConnector(QWidget *parent)
    : QWidget(parent)
{
}

CodeEditor *SegmentConnector::left() const
{
    return _left;
}

void SegmentConnector::setLeft(CodeEditor *newLeft)
{
    _left = newLeft;
}

CodeEditor *SegmentConnector::right() const
{
    return _right;
}

void SegmentConnector::setRight(CodeEditor *newRight)
{
    _right = newRight;
}

void SegmentConnector::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!_left || !_right)
        return;

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(rect(), Qt::white);

    for (auto s = mSegmentPos.begin(); s != mSegmentPos.end(); ++s) {
        if (s.key()->type == Diff::SegmentType::SameOnBoth)
            continue;
        auto leftArea = _left->blockArea(s->leftStart, s->leftEnd);
        auto rightArea = _right->blockArea(s->rightStart, s->rightEnd);

        //        if (s == _currentSegment)
        //            painter.setBrush(Qt::yellow);
        //        else
        switch (s.key()->type) {
        case Diff::SegmentType::SameOnBoth:
            //            painter.setBrush(Qt::magenta);
            continue;
        case Diff::SegmentType::OnlyOnRight:
            painter.setBrush(GitKlientSettings::diffAddedColor());
            break;
        case Diff::SegmentType::OnlyOnLeft:
            painter.setBrush(GitKlientSettings::diffRemovedColor());
            break;
        case Diff::SegmentType::DifferentOnBoth:
            painter.setBrush(GitKlientSettings::diffModifiedColor());
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
