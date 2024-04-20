/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reportswidget.h"

#include <gitmanager.h>
#include <reports/authorsreport.h>
#include <reports/commitsbydayhour.h>
#include <reports/commitsbydayweek.h>
#include <reports/commitsbymonth.h>
#include <widgets/reportwidget.h>

ReportsWidget::ReportsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);

    addReport(new AuthorsReport{git, this});
    addReport(new CommitsByDayWeek{git, this});
    addReport(new CommitsByDayHour{git, this});
    addReport(new CommitsByMonth{git, this});

    connect(git, &Git::Manager::reloadRequired, this, &ReportsWidget::reloadReports);
    connect(pushButtonRefresh, &QPushButton::clicked, this, &ReportsWidget::reloadReports);
    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    stackedWidget->setCurrentIndex(0);

    connect(pushButtonTable, &QPushButton::clicked, this, &ReportsWidget::mSlotPushButtonTableClicked);
    connect(pushButtonChart, &QPushButton::clicked, this, &ReportsWidget::mSlotPushButtonChartClicked);

    mSlotPushButtonTableClicked();
}

void ReportsWidget::saveState(QSettings &settings) const
{
    save(settings, splitter);
}

void ReportsWidget::restoreState(QSettings &settings)
{
    restore(settings, splitter);
}

void ReportsWidget::reloadReports()
{
    for (auto &report : mReportWidgets)
        report->reload();
}

void ReportsWidget::mSlotPushButtonTableClicked()
{
    pushButtonTable->setChecked(true);
    pushButtonChart->setChecked(false);
    for (auto &reportWidget : mReportWidgets)
        reportWidget->setShowMode(ReportWidget::Table);
}

void ReportsWidget::mSlotPushButtonChartClicked()
{
    pushButtonTable->setChecked(false);
    pushButtonChart->setChecked(true);
    for (auto &reportWidget : mReportWidgets)
        reportWidget->setShowMode(ReportWidget::Chart);
}

void ReportsWidget::addReport(AbstractReport *report)
{
    auto reportWidget = new ReportWidget{report, stackedWidget};

    stackedWidget->addWidget(reportWidget);
    listWidget->addItem(report->name());

    mReportWidgets << reportWidget;
}
