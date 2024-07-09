/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "graphpainter.h"

#include "entities/commit.h"
#include "models/commitsmodel.h"
#include <QPainter>
#include <QPainterPath>

#include <KLocalizedString>

#define HEIGHT 25
#define WIDTH 18

int drawReference(QPainter *painter, QSharedPointer<Git::Reference> reference, int lanesSize)
{
    QString refStr;
    if (reference->isBranch())
        refStr = i18n("Branch: ");
    else if (reference->isNote())
        refStr = i18n("Note: ");
    else if (reference->isRemote())
        refStr = i18n("Remote: ");
    else if (reference->isTag())
        refStr = i18n("Tag: ");

    const auto ref = refStr + reference->shorthand();
    QRect rcBox(lanesSize * WIDTH, 0, painter->fontMetrics().horizontalAdvance(ref) + 8, painter->fontMetrics().height() + 4);
    rcBox.moveTop((HEIGHT - rcBox.height()) / 2);

    // QLinearGradient linearGrad(rcBox.topLeft(), rcBox.bottomRight());
    // linearGrad.setFinalStop(rcBox.bottomLeft());
    // linearGrad.setColorAt(0, Qt::white);
    // linearGrad.setColorAt(1, QColor(100, 100, 255));

    // painter->fillRect(rcBox.left(), rcBox.top(), painter->fontMetrics().horizontalAdvance(refStr) + 2, rcBox.height(), QBrush(linearGrad));

    /*painter->fillRect(rcBox.left() + painter->fontMetrics().horizontalAdvance(refStr) + 2,
                      rcBox.top(),
                      rcBox.width() - painter->fontMetrics().horizontalAdvance(refStr) - 2,
                      rcBox.height(),
                      Qt::white);*/

    painter->setBrush(Qt::transparent);
    painter->drawRoundedRect(rcBox, 5, 5);
    painter->drawText(rcBox, Qt::AlignVCenter | Qt::AlignHCenter, ref);
    return rcBox.width();
}

QPoint center(int x)
{
    return {(x * WIDTH) + (WIDTH / 2), HEIGHT / 2};
}
QPoint centerEdge(int x, Qt::Edge edge)
{
    switch (edge) {
    case Qt::TopEdge:
        return {(x * WIDTH) + (WIDTH / 2), 0};
    case Qt::LeftEdge:
        return {(x * WIDTH), HEIGHT / 2};
    case Qt::RightEdge:
        return {(x + 1) * WIDTH, HEIGHT / 2};
    case Qt::BottomEdge:
        return {(x * WIDTH) + (WIDTH / 2), HEIGHT};
    }
    return {};
}

QPoint point(int col, Qt::Alignment align = Qt::AlignCenter)
{
    int y;
    int x = col * WIDTH;

    if (align & Qt::AlignTop)
        y = 0;
    else if (align & Qt::AlignBottom)
        y = HEIGHT;
    else
        y = HEIGHT / 2;

    if (align & Qt::AlignLeft)
        x += 0;
    else if (align & Qt::AlignRight)
        x += WIDTH;
    else
        x += WIDTH / 2;

    return {x, y};
}
QPoint centerGuide(int x, Qt::Edge edge)
{
    constexpr const int tel{5};

    QPoint pt = point(x);
    switch (edge) {
    case Qt::TopEdge:
        pt.setY(pt.y() - tel);
        break;
    case Qt::LeftEdge:
        pt.setX(pt.x() - tel);
        break;
    case Qt::RightEdge:
        pt.setX(pt.x() + tel);
        break;
    case Qt::BottomEdge:
        pt.setY(pt.y() + tel);
        break;
    }
    return pt;
}

GraphPainter::GraphPainter(CommitsModel *model, QObject *parent)
    : QStyledItemDelegate(parent)
    , mModel(model)
{
    mColors = {Qt::red, Qt::blue, Qt::darkGreen, Qt::magenta, Qt::darkMagenta, Qt::darkBlue, Qt::darkBlue, Qt::darkRed, Qt::darkYellow, Qt::darkGreen};
}

void GraphPainter::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto lanes = mModel->lanesFromIndex(index);
    auto log = mModel->fromIndex(index);

    painter->setRenderHints(QPainter::Antialiasing);

    //    painter->fillRect(option.rect, option.palette.base());
    if (option.state & QStyle::State_Selected)
        painter->fillRect(option.rect, option.palette.highlight());
    //    else if (option.state & QStyle::State_MouseOver)
    //        painter->fillRect(option.rect, option.palette.brush(QPalette::Normal, QPalette::Highlight));
    //    else if (index.row() & 1)
    //        painter->fillRect(option.rect, option.palette.alternateBase());
    else
        painter->fillRect(option.rect, option.palette.base());

    painter->save();
    painter->setClipRect(option.rect, Qt::IntersectClip);
    painter->translate(option.rect.topLeft());

    int x{-1};
    for (auto &l : lanes) {
        ++x;
        if (l.type() == GraphLane::None)
            continue;

        if (x >= mColors.size()) {
            painter->setPen(Qt::black);
            painter->setBrush(Qt::black);
        } else {
            painter->setPen(mColors.at(x));
            painter->setBrush(mColors.at(x));
        }
        //        painter->setPen(l.color());
        //        painter->setBrush(l.color());
        paintLane(painter, l, x);
    }

    QRect rc(lanes.size() * WIDTH, 0, painter->fontMetrics().horizontalAdvance(log->message()), HEIGHT);

    painter->setPen(option.palette.color(QPalette::Text));
    auto refs = log->reference();
    for (auto const &ref : refs) {
        auto w = drawReference(painter, ref, lanes.size());
        rc.moveLeft(rc.left() + w + 6);
    }
    painter->drawText(rc, Qt::AlignVCenter, log->message());

    painter->restore();
}

void GraphPainter::paintLane(QPainter *painter, const GraphLane &lane, int index) const
{
    switch (lane.type()) {
    case GraphLane::Start:
        painter->drawLine(point(index), point(index, Qt::AlignTop));
        painter->setBrush(Qt::white);
        painter->drawEllipse(point(index), 3, 3);
        break;
    case GraphLane::Pipe:
        painter->drawLine(point(index, Qt::AlignTop), point(index, Qt::AlignBottom));
        break;
    case GraphLane::Node:
        painter->drawLine(point(index, Qt::AlignTop), point(index, Qt::AlignBottom));
        painter->setBrush(Qt::white);
        painter->drawEllipse(point(index), 3, 3);
        break;
    case GraphLane::End:
        painter->drawLine(point(index), point(index, Qt::AlignBottom));
        painter->setBrush(Qt::white);
        painter->drawEllipse(point(index), 3, 3);
        break;
    case GraphLane::Test:
        painter->drawLine(point(index, Qt::AlignTop | Qt::AlignLeft), point(index, Qt::AlignBottom | Qt::AlignRight));
        painter->drawLine(point(index, Qt::AlignTop | Qt::AlignRight), point(index, Qt::AlignBottom | Qt::AlignLeft));
        break;

    case GraphLane::None:
    case GraphLane::Transparent:
        break; // just to avoid compiler warning
    }

    for (const auto &i : lane.upJoins()) {
        painter->drawEllipse(point(i), 2, 2);
        QPainterPath p;
        p.moveTo(point(i));
        p.cubicTo(centerGuide(index, Qt::LeftEdge), centerGuide(index, Qt::TopEdge), point(index, Qt::AlignTop));
        painter->setBrush(Qt::transparent);
        painter->drawPath(p);
    }
    for (const auto &i : lane.bottomJoins()) {
        painter->drawEllipse(point(i), 2, 2);

        QPainterPath p;
        p.moveTo(point(index, Qt::AlignBottom));
        p.cubicTo(centerGuide(index, Qt::BottomEdge), centerGuide(index, Qt::LeftEdge), point(i));
        painter->setBrush(Qt::transparent);
        //        painter->setPen(Qt::DotLine);
        painter->drawPath(p);
    }
}

int GraphPainter::colX(int col) const
{
    return col * WIDTH;
}

QSize GraphPainter::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return {0, HEIGHT};
}

#include "moc_graphpainter.cpp"
