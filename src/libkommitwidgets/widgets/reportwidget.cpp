/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#if QT_CHARTS_LIB
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
}

void ReportWidget::initChart()
{
#if QT_CHARTS_LIB
    if (!mReport->supportChart())
        return;
    using namespace QtCharts;

    auto chart = new QChart;
    barSeries = new QBarSeries;

    chart->addSeries(barSeries);
    axisX = new QBarCategoryAxis();
    axisX->setTitleText("Hour");
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

#else
    stackedWidget->setCurrentIndex(0);
#endif
}

void ReportWidget::fillTableWidget()
{
    tableWidget->clear();
    tableWidget->setRowCount(mReport->rowCount());
    tableWidget->setColumnCount(mReport->columnCount());

    for (int row = 0; row < mReport->rowCount(); ++row) {
        for (auto col = 0; col < mReport->columnCount(); ++col) {
            auto item = new QTableWidgetItem(mReport->at(row, col).toString());
            tableWidget->setItem(row, col, item);
        }
    }
}

void ReportWidget::fillChart()
{
    using namespace QtCharts;

    auto barSet = new QBarSet(mReport->name());
    axisX->clear();

    QStringList names;
    for (int row = 0; row < mReport->rowCount(); ++row) {
        barSet->append(mReport->at(row, mReport->valueColumn()).toReal());
        names << mReport->at(row, mReport->categoryColumn()).toString();
    }
    axisX->append(names);

    barSeries->clear();
    barSeries->append(barSet);

    axis->setMin(mReport->minValue());
    axis->setMax(mReport->maxValue());
}

int ReportWidget::showMode() const
{
    return static_cast<ShowMode>(stackedWidget->currentIndex());
}

void ReportWidget::setShowMode(int showMode)
{
    if (!mReport->supportChart())
        return;
    stackedWidget->setCurrentIndex(static_cast<int>(showMode));
}

void ReportWidget::reload()
{
    mReport->reload();

    fillTableWidget();
    fillChart();
}
