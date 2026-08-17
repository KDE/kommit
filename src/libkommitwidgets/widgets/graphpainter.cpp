/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "graphpainter.h"
#include <Kommit/BranchesCache>
#include <Kommit/Repository>

#include "entities/commit.h"
#include "models/commitsmodel.h"
#include <QPainter>
#include <QPainterPath>

#include <KLocalizedString>

#define HEIGHT 25
#define WIDTH 18

namespace Sizes
{
constexpr const int dotSize{3};
}

namespace
{
/// The colour every repository's main line is drawn in, whatever else is in it.
const QColor DefaultBranchColor{Qt::red};

/// A branch without the remote it came from, so a branch and its remote share a colour.
QString branchName(const QString &shorthand)
{
    const auto slash = shorthand.lastIndexOf(QLatin1Char('/'));

    return slash == -1 ? shorthand : shorthand.mid(slash + 1);
}

/**
 * The colour of @p branch, the same one every time and everywhere it is drawn.
 *
 * Red is kept for the line the history is built on, so it is recognisable at a glance in
 * any repository, and the rest are told apart by the name they are given.
 */
QColor branchColor(const QString &branch, const QString &defaultBranch)
{
    static const QVector<QColor> colors{Qt::blue,
                                        Qt::darkGreen,
                                        Qt::magenta,
                                        Qt::darkCyan,
                                        Qt::darkYellow,
                                        Qt::darkBlue,
                                        Qt::darkMagenta,
                                        QColor{0xE9, 0x6D, 0x00},
                                        QColor{0x00, 0x7A, 0x7A},
                                        QColor{0x6A, 0x3D, 0x9A}};

    const auto name = branchName(branch);
    if (name.isEmpty())
        return {};

    if (!defaultBranch.isEmpty() && name == branchName(defaultBranch))
        return DefaultBranchColor;

    return colors.at(static_cast<int>(qHash(name) % static_cast<size_t>(colors.size())));
}

/// Black or white, whichever stands out against @p background.
QColor readableTextColor(const QColor &background)
{
    // Rec. 601 luminance, the weighting qGray uses.
    const auto luminance = (background.redF() * 299 + background.greenF() * 587 + background.blueF() * 114) / 1000;

    return luminance > 0.5 ? QColor{Qt::black} : QColor{Qt::white};
}
}
class GraphPainterPrivate
{
    GraphPainter *q_ptr;
    Q_DECLARE_PUBLIC(GraphPainter)

public:
    GraphPainterPrivate(GraphPainter *parent, CommitsModel *model);

    CommitsModel *const model;
    QVector<QColor> colors;
    /// Asked of the repository once, and again whenever the model is read afresh.
    mutable QString defaultBranch;

    [[nodiscard]] QString defaultBranchName() const;

    int colX(int col) const;
    void paintLane(QPainter *painter, const GraphLane &lane, int index) const;
    void drawReference(QPainter *painter, const Git::Reference &reference, int &x) const;
    QPoint center(int x) const;
    QPoint centerEdge(int x, Qt::Edge edge) const;
    QPoint point(int col, Qt::Alignment align = Qt::AlignCenter) const;
    QPoint centerGuide(int x, Qt::Edge edge) const;
    void paintPathToTop(QPainter *painter, int from, int to) const;
    void paintPathToDown(QPainter *painter, int from, int to) const;
};

QString GraphPainterPrivate::defaultBranchName() const
{
    if (defaultBranch.isEmpty() && model && model->manager())
        defaultBranch = model->manager()->branches()->defaultName();

    return defaultBranch;
}

GraphPainter::GraphPainter(CommitsModel *model, QObject *parent)
    : QStyledItemDelegate(parent)
    , d_ptr{new GraphPainterPrivate{this, model}}
{
    Q_D(GraphPainter);

    // Another repository, or the same one read again, may be built on another branch.
    connect(model, &QAbstractItemModel::modelReset, this, [d] {
        d->defaultBranch.clear();
    });
}

GraphPainter::~GraphPainter()
{
    Q_D(GraphPainter);
    delete d;
}

void GraphPainter::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_D(const GraphPainter);
    auto lanes = d->model->lanesFromIndex(index);
    auto log = d->model->fromIndex(index);

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

        // The colour of the branch the line carries, and failing that one told apart from its
        // neighbours by where it sits.
        auto color = branchColor(l.branch(), d->defaultBranchName());
        if (!color.isValid())
            color = x < d->colors.size() ? d->colors.at(x) : QColor{Qt::black};

        painter->setPen(color);
        painter->setBrush(color);
        //        painter->setPen(l.color());
        //        painter->setBrush(l.color());
        d->paintLane(painter, l, x);
    }

    QRect rc(lanes.size() * WIDTH, 0, painter->fontMetrics().horizontalAdvance(log.summary()), HEIGHT);

    painter->setPen(option.palette.color(QPalette::Text));
    auto refs = log.references();
    int refBoxX = lanes.size() * WIDTH;
    for (auto const &ref : refs) {
        d->drawReference(painter, ref, refBoxX);
    }
    rc.moveLeft(refBoxX + 6);
    painter->drawText(rc, Qt::AlignVCenter, log.summary());

    painter->restore();
}

QSize GraphPainter::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)
    return {0, HEIGHT};
}

GraphPainterPrivate::GraphPainterPrivate(GraphPainter *parent, CommitsModel *model)
    : q_ptr{parent}
    , model{model}
{
    colors = {Qt::red, Qt::blue, Qt::darkGreen, Qt::magenta, Qt::darkMagenta, Qt::darkBlue, Qt::darkBlue, Qt::darkRed, Qt::darkYellow, Qt::darkGreen};
}

void GraphPainterPrivate::drawReference(QPainter *painter, const Git::Reference &reference, int &x) const
{
    QString refStr;
    if (reference.isBranch())
        refStr = i18n("Branch: ");
    else if (reference.isNote())
        refStr = i18n("Note: ");
    else if (reference.isRemote())
        refStr = i18n("Remote: ");
    else if (reference.isTag())
        refStr = i18n("Tag: ");

    const auto ref = refStr + reference.shorthand();
    QRect rcBox(x, 0, painter->fontMetrics().horizontalAdvance(ref) + 8, painter->fontMetrics().height() + 4);
    rcBox.moveTop((HEIGHT - rcBox.height()) / 2);

    // The name of a branch on the colour of its line, so the two read as the same thing.
    const auto color = branchColor(reference.shorthand(), defaultBranchName());

    painter->save();
    if (color.isValid()) {
        painter->setPen(color);
        painter->setBrush(color);
        painter->drawRoundedRect(rcBox, 5, 5);
        painter->setPen(readableTextColor(color));
    } else {
        painter->setBrush(Qt::transparent);
        painter->drawRoundedRect(rcBox, 5, 5);
    }

    painter->drawText(rcBox, Qt::AlignVCenter | Qt::AlignHCenter, ref);
    painter->restore();

    x += rcBox.width() + 4;
}

int GraphPainterPrivate::colX(int col) const
{
    return col * WIDTH;
}

void GraphPainterPrivate::paintLane(QPainter *painter, const GraphLane &lane, int index) const
{
    switch (lane.type()) {
    case GraphLane::Start:
        painter->drawLine(point(index), point(index, Qt::AlignTop));
        painter->setBrush(Qt::white);
        painter->drawEllipse(point(index), Sizes::dotSize, Sizes::dotSize);
        break;
    case GraphLane::Pipe:
        painter->drawLine(point(index, Qt::AlignTop), point(index, Qt::AlignBottom));
        break;
    case GraphLane::Node:
        painter->drawLine(point(index, Qt::AlignTop), point(index, Qt::AlignBottom));
        painter->setBrush(Qt::white);
        painter->drawEllipse(point(index), Sizes::dotSize, Sizes::dotSize);
        break;
    case GraphLane::End:
        painter->drawLine(point(index), point(index, Qt::AlignBottom));
        painter->setBrush(Qt::white);
        painter->drawEllipse(point(index), Sizes::dotSize, Sizes::dotSize);
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
        paintPathToTop(painter, i, index);
    }
    for (const auto &i : lane.bottomJoins()) {
        // painter->drawEllipse(point(i), 2, 2);
        paintPathToDown(painter, i, index);
    }
}

QPoint GraphPainterPrivate::center(int x) const
{
    return {(x * WIDTH) + (WIDTH / 2), HEIGHT / 2};
}
QPoint GraphPainterPrivate::centerEdge(int x, Qt::Edge edge) const
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

QPoint GraphPainterPrivate::point(int col, Qt::Alignment align) const
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
QPoint GraphPainterPrivate::centerGuide(int x, Qt::Edge edge) const
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

void GraphPainterPrivate::paintPathToTop(QPainter *painter, int from, int to) const
{
    QPainterPath p;

    if (from < to) {
        p.moveTo(centerGuide(from, Qt::RightEdge));
        p.lineTo(centerEdge(to, Qt::LeftEdge));
        p.cubicTo(centerGuide(to, Qt::LeftEdge), centerGuide(to, Qt::TopEdge), centerEdge(to, Qt::TopEdge));

    } else {
        p.moveTo(centerGuide(from, Qt::LeftEdge));
        p.lineTo(centerEdge(to, Qt::RightEdge));
        p.cubicTo(centerGuide(to, Qt::RightEdge), centerGuide(to, Qt::TopEdge), centerEdge(to, Qt::TopEdge));
    }
    painter->setBrush(Qt::transparent);
    painter->drawPath(p);
}

void GraphPainterPrivate::paintPathToDown(QPainter *painter, int from, int to) const
{
    QPainterPath p;

    if (from < to) {
        p.moveTo(centerGuide(from, Qt::RightEdge));
        p.lineTo(centerEdge(to, Qt::LeftEdge));
        p.cubicTo(centerGuide(to, Qt::LeftEdge), centerGuide(to, Qt::BottomEdge), centerEdge(to, Qt::BottomEdge));
    } else {
        p.moveTo(centerGuide(from, Qt::LeftEdge));
        p.lineTo(centerEdge(to, Qt::RightEdge));
        p.cubicTo(centerGuide(to, Qt::RightEdge), centerGuide(to, Qt::BottomEdge), centerEdge(to, Qt::BottomEdge));
    }
    painter->setBrush(Qt::transparent);
    painter->drawPath(p);
}

#include "moc_graphpainter.cpp"
