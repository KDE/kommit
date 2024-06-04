/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

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
    // treeWidget->setSortingEnabled(true);
    // treeWidget->header()->setSortIndicatorShown(true);
    // treeWidget->header()->setSectionsClickable(true);

    initChart();
}

void ReportWidget::initChart()
{
#ifdef QT_CHARTS_LIB
    if (!mReport->supportChart())
        return;

    auto chart = new QChart;
    barSeries = new QBarSeries;

    chart->addSeries(barSeries);
    axisX = new QBarCategoryAxis();
    axisX->append(mReport->headerData());
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    axis = new QValueAxis();
    chart->addAxis(axis, Qt::AlignLeft);
    barSeries->attachAxis(axis);

    auto chartView = new QChartView{chart, stackedWidget};
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumSize(640, 480);

    stackedWidget->addWidget(chartView);
    // stackedWidget->setCurrentIndex(1);
    chartView->show();
    chart->setTitle(mReport->name());

    chart->legend()->hide();
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

    QStringList names;
    for (int row = 0; row < mReport->rowCount(); ++row) {
        barSet->append(mReport->at(row, mReport->valueColumn()).toReal());
        names << mReport->at(row, mReport->categoryColumn()).toString();
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
