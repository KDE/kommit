/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "segmentsscrollbar.h"

#include "segmentconnector.h"
#include "widgets/codeeditor.h"

#include <QPaintEvent>
#include <QPainter>
#include <kommitwidgetsglobaloptions.h>

class SegmentsScrollBarPrivate
{
    SegmentsScrollBar *q_ptr;
    Q_DECLARE_PUBLIC(SegmentsScrollBar)

public:
    SegmentsScrollBarPrivate(SegmentsScrollBar *parent);
    enum Side { Left, Right };

    int leftCount{0};
    int rightCount{0};
    SegmentConnector *mSegmentConnector{nullptr};
    QImage mSegmentsImage;

    void generateSegmentsImage();

    inline void paintSection(QPainter &painter, Side side, int from, int len, const QBrush &brush, bool drawRect = false);
};

SegmentsScrollBar::SegmentsScrollBar(QWidget *parent)
    : QWidget{parent}
    , d_ptr{new SegmentsScrollBarPrivate{this}}
{
    setMouseTracking(true);
}

SegmentsScrollBar::~SegmentsScrollBar()
{
    Q_D(SegmentsScrollBar);
    delete d;
}

SegmentConnector *SegmentsScrollBar::segmentConnector() const
{
    Q_D(const SegmentsScrollBar);
    return d->mSegmentConnector;
}

void SegmentsScrollBar::setSegmentConnector(SegmentConnector *newSegmentConnector)
{
    Q_D(SegmentsScrollBar);

    if (d->mSegmentConnector) {
        disconnect(newSegmentConnector, &SegmentConnector::segmentsChanged, this, &SegmentsScrollBar::reload);
        disconnect(newSegmentConnector, &SegmentConnector::sameSizeChanged, this, &SegmentsScrollBar::reload);
    }
    d->mSegmentConnector = newSegmentConnector;
    connect(newSegmentConnector, &SegmentConnector::segmentsChanged, this, &SegmentsScrollBar::reload);
    connect(newSegmentConnector, &SegmentConnector::sameSizeChanged, this, &SegmentsScrollBar::reload);
    update();
}

void SegmentsScrollBar::paintEvent(QPaintEvent *event)
{
    Q_D(SegmentsScrollBar);

    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::white);

    if (!Q_UNLIKELY(d->mSegmentConnector))
        return;

    painter.drawImage(0, 0, d->mSegmentsImage);

    auto leftArea = d->mSegmentConnector->left()->visibleLines();
    auto rightArea = d->mSegmentConnector->right()->visibleLines();

    QBrush br(Qt::gray);

    painter.fillRect(QRect{int(width() * .4), 0, int(width() * .2), height() - 1}, palette().base());
    painter.drawLine(width() * .4, 0, width() * .4, height() - 1);
    painter.drawLine(width() * .6, 0, width() * .6, height() - 1);

    painter.setOpacity(.3);
    d->paintSection(painter, SegmentsScrollBarPrivate::Side::Left, leftArea.first, leftArea.second, br, true);
    d->paintSection(painter, SegmentsScrollBarPrivate::Side::Right, rightArea.first, rightArea.second, br, true);
    painter.setOpacity(1);
    painter.drawRect(QRect{0, 0, width() - 1, height() - 1});
}

void SegmentsScrollBar::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    event->accept();
}

void SegmentsScrollBar::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);

    if (event->buttons() & Qt::LeftButton)
        Q_EMIT mouseMove(event->y(), static_cast<double>(event->y()) / static_cast<double>(height()));
    else
        Q_EMIT hover(event->y(), static_cast<double>(event->y()) / static_cast<double>(height()));
}

void SegmentsScrollBar::resizeEvent(QResizeEvent *event)
{
    Q_D(SegmentsScrollBar);
    Q_UNUSED(event)
    if (Q_LIKELY(d->mSegmentConnector))
        d->generateSegmentsImage();
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void SegmentsScrollBar::enterEvent(QEvent *event)
#else
void SegmentsScrollBar::enterEvent(QEnterEvent *event)
#endif
{
    QWidget::enterEvent(event);
    Q_EMIT mouseEntered();
}

void SegmentsScrollBar::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    Q_EMIT mouseLeaved();
}

void SegmentsScrollBar::reload()
{
    Q_D(SegmentsScrollBar);
    d->leftCount = d->rightCount = 0;
    for (const auto &segment : qAsConst(d->mSegmentConnector->segments())) {
        if (d->mSegmentConnector->sameSize()) {
            auto m = qMax(segment->oldText.size(), segment->newText.size());
            d->leftCount += m;
            d->rightCount += m;
        } else {
            d->leftCount += segment->oldText.size();
            d->rightCount += segment->newText.size();
        }
    }
    update();
}

void SegmentsScrollBarPrivate::generateSegmentsImage()
{
    Q_Q(SegmentsScrollBar);
    mSegmentsImage = QImage{q->width(), q->height(), QImage::Format_ARGB32};

    mSegmentsImage.fill(Qt::white);
    QPainter painter{&mSegmentsImage};

    int countLeft{0};
    int countRight{0};

    for (const auto &segment : qAsConst(mSegmentConnector->segments())) {
        QBrush brush;
        switch (segment->type) {
        case Diff::SegmentType::OnlyOnLeft:
            brush = KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Removed);
            break;
        case Diff::SegmentType::OnlyOnRight:
            brush = KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Added);
            break;
        case Diff::SegmentType::SameOnBoth:
            brush = Qt::white;
            break;
        case Diff::SegmentType::DifferentOnBoth:
            brush = KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Modified);
            break;
        default:
            break;
        }

        paintSection(painter, Side::Left, countLeft, segment->oldText.size(), brush);
        paintSection(painter, Side::Right, countRight, segment->newText.size(), brush);

        if (mSegmentConnector->sameSize()) {
            auto m = qMax(segment->oldText.size(), segment->newText.size());

            paintSection(painter, Side::Left, countLeft + segment->oldText.size(), m - segment->oldText.size(), Qt::darkGray);
            paintSection(painter, Side::Right, countRight + segment->newText.size(), m - segment->newText.size(), Qt::darkGray);

            countLeft += m;
            countRight += m;
        } else {
            countLeft += segment->oldText.size();
            countRight += segment->newText.size();
        }
    }
    /*
        auto leftArea = d->mSegmentConnector->left()->visibleLines();
        auto rightArea = d->mSegmentConnector->right()->visibleLines();

        QBrush br(Qt::gray);

        painter.fillRect(QRect{int(width() * .4), 0, int(width() * .2), height() - 1}, palette().base());
        painter.drawLine(width() * .4, 0, width() * .4, height() - 1);
        painter.drawLine(width() * .6, 0, width() * .6, height() - 1);

        painter.setOpacity(.3);
        paintSection(painter, Left, leftArea.first, leftArea.second, br, true);
        paintSection(painter, Right, rightArea.first, rightArea.second, br, true);
        painter.setOpacity(1);
        painter.drawRect(QRect{0, 0, width() - 1, height() - 1});*/
}

void SegmentsScrollBarPrivate::paintSection(QPainter &painter, Side side, int from, int len, const QBrush &brush, bool drawRect)
{
    Q_Q(SegmentsScrollBar);
    if (len <= 0)
        return;
    int w = q->width() * .4;
    int l = side == Left ? 0 : q->width() - w;
    int &c = side == Left ? leftCount : rightCount;

    QRect rc{l, static_cast<int>((from / (double)c) * q->height()) - 1, w, static_cast<int>((len / (double)c) * q->height())};
    painter.fillRect(rc, brush);
    if (drawRect)
        painter.drawRect(rc);
}

#include "moc_segmentsscrollbar.cpp"

SegmentsScrollBarPrivate::SegmentsScrollBarPrivate(SegmentsScrollBar *parent)
    : q_ptr{parent}
{
}
