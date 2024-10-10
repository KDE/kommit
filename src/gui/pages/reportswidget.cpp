/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reportswidget.h"

#include <core/repositorydata.h>
#include <repository.h>
#include <reports/authorsreport.h>
#include <reports/commitsbydayhour.h>
#include <reports/commitsbydayweek.h>
#include <reports/commitsbymonth.h>
#include <widgets/reportwidget.h>

ReportsWidget::ReportsWidget(RepositoryData *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);

    addReport(new AuthorsReport{git->manager(), this});
    addReport(new CommitsByDayWeek{git->manager(), this});
    addReport(new CommitsByDayHour{git->manager(), this});
    addReport(new CommitsByMonth{git->manager(), this});

    connect(git->manager(), &Git::Repository::reloadRequired, this, &ReportsWidget::reloadReports);
    connect(toolButtonRefresh, &QToolButton::clicked, this, &ReportsWidget::reloadReports);
    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    stackedWidget->setCurrentIndex(0);

    connect(toolButtonTable, &QToolButton::clicked, this, &ReportsWidget::slotToolButtonTableClicked);
    connect(toolButtonChart, &QToolButton::clicked, this, &ReportsWidget::slotToolButtonChartClicked);

    slotToolButtonTableClicked();

#ifndef QT_CHARTS_LIB
    toolButtonTable->hide();
    toolButtonChart->hide();
#endif
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
    for (auto &report : std::as_const(mReportWidgets))
        report->reload();
}

void ReportsWidget::slotToolButtonTableClicked()
{
    toolButtonTable->setChecked(true);
    toolButtonChart->setChecked(false);
    for (auto &reportWidget : std::as_const(mReportWidgets))
        reportWidget->setShowMode(ReportWidget::Table);
}

void ReportsWidget::slotToolButtonChartClicked()
{
    toolButtonTable->setChecked(false);
    toolButtonChart->setChecked(true);
    for (auto &reportWidget : std::as_const(mReportWidgets))
        reportWidget->setShowMode(ReportWidget::Chart);
}

void ReportsWidget::addReport(AbstractReport *report)
{
    auto reportWidget = new ReportWidget{report, stackedWidget};

    stackedWidget->addWidget(reportWidget);
    listWidget->addItem(report->name());

    mReportWidgets << reportWidget;
}

#include "moc_reportswidget.cpp"
