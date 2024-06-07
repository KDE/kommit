/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include <QShortcut>

#ifdef QT_CHARTS_LIB
#include <QBarCategoryAxis>
#include <QBarSeries>
#include <QBarSet>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QVXYModelMapper>
#include <QValueAxis>
#endif

#include "reportwidget.h"

#include "reports/abstractreport.h"

ReportWidget::ReportWidget(AbstractReport *report, QWidget *parent)
    : QWidget(parent)
    , mReport{report}
{
    setupUi(this);

    initChart();
    auto zoomInShortcut = new QShortcut(QKeySequence(QKeySequence::ZoomIn), this);
    zoomInShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(zoomInShortcut, &QShortcut::activated, this, &ReportWidget::slotZoomIn);

    auto zoomOutShortcut = new QShortcut(QKeySequence(QKeySequence::ZoomOut), this);
    zoomOutShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(zoomOutShortcut, &QShortcut::activated, this, &ReportWidget::slotZoomOut);

    auto zoomResetShortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_0), this);
    zoomResetShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    connect(zoomResetShortcut, &QShortcut::activated, this, &ReportWidget::zoomResetShortcut);
}

void ReportWidget::zoomResetShortcut()
{
#ifdef QT_CHARTS_LIB
    if (!mReport->supportChart())
        return;
    mChart->zoomReset();
#endif
}

void ReportWidget::slotZoomIn()
{
#ifdef QT_CHARTS_LIB
    if (!mReport->supportChart())
        return;
    mChart->zoomIn();
#endif
}

void ReportWidget::slotZoomOut()
{
#ifdef QT_CHARTS_LIB
    if (!mReport->supportChart())
        return;
    mChart->zoomOut();
#endif
}

void ReportWidget::initChart()
{
#ifdef QT_CHARTS_LIB
    if (!mReport->supportChart())
        return;

    mChart = new QChart;
    barSeries = new QBarSeries;

    mChart->addSeries(barSeries);
    axisX = new QBarCategoryAxis();
    axisX->append(mReport->headerData());
    mChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    axis = new QValueAxis();
    mChart->addAxis(axis, Qt::AlignLeft);
    barSeries->attachAxis(axis);

    auto chartView = new QChartView{mChart, stackedWidget};
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    stackedWidget->addWidget(chartView);
    // stackedWidget->setCurrentIndex(1);
    chartView->show();
    mChart->setTitle(mReport->name());

    mChart->legend()->hide();
#else
    stackedWidget->setCurrentIndex(0);
#endif
}

void ReportWidget::fillTableWidget()
{
    treeWidget->clear();
    treeWidget->setColumnCount(mReport->columnCount());
    treeWidget->setHeaderLabels(mReport->headerData());

    for (int row = 0; row < mReport->rowCount(); ++row) {
        auto item = new QTreeWidgetItem(treeWidget);
        for (auto col = 0; col < mReport->columnCount(); ++col) {
            item->setText(col, mReport->at(row, col).toString());
        }
    }
}

void ReportWidget::fillChart()
{
#ifdef QT_CHARTS_LIB
    auto barSet = new QBarSet{mReport->name(), this};
    axisX->clear();

    // QChart doesn't support duplicate entries

    struct Info {
        QString name;
        qreal value = 0;
    };
    QList<Info> listElements;
    for (int row = 0; row < mReport->rowCount(); ++row) {
        const QString name = mReport->at(row, mReport->categoryColumn()).toString();
        const qreal value = mReport->at(row, mReport->valueColumn()).toReal();
        auto i = std::find_if(listElements.begin(), listElements.end(), [name](const Info &info) {
            return info.name == name;
        });
        if (i == listElements.end()) {
            listElements.append(Info{name, value});
        } else {
            (*i).value += value;
        }
    }
    QStringList names;
    for (const Info &info : listElements) {
        barSet->append(info.value);
        names << info.name;
    }
    axisX->append(names);
    axisX->setLabelsAngle(mReport->labelsAngle());

    barSeries->clear();
    barSeries->append(barSet);

    axis->setMin(mReport->minValue());
    axis->setMax(mReport->maxValue());
    axis->setTitleText(mReport->axisYTitle());
#endif
}

int ReportWidget::showMode() const
{
    return stackedWidget->currentIndex();
}

void ReportWidget::setShowMode(int showMode)
{
    if (!mReport->supportChart())
        return;
    stackedWidget->setCurrentIndex(showMode);
}

void ReportWidget::reload()
{
    mReport->reload();

    fillTableWidget();
    fillChart();
}

#include "moc_reportwidget.cpp"
