#include "reportswidget.h"

#include <gitmanager.h>
#include <reports/authorsreport.h>
#include <reports/commitsbydayhour.h>
#include <reports/commitsbydayweek.h>
#include <widgets/reportwidget.h>

ReportsWidget::ReportsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);

    addReport(new AuthorsReport{git, this});
    addReport(new CommitsByDayWeek{git, this});
    addReport(new CommitsByDayHour{git, this});

    connect(git, &Git::Manager::reloadRequired, this, &ReportsWidget::reloadReports);
    connect(pushButtonRefresh, &QPushButton::clicked, this, &ReportsWidget::reloadReports);
    connect(listWidget, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex);

    stackedWidget->setCurrentIndex(0);
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
    for (auto &report : mReports)
        report->reload();
}

void ReportsWidget::addReport(AbstractReport *report)
{
    mReports << report;
    auto reportWidget = new ReportWidget{report, stackedWidget};

    stackedWidget->addWidget(reportWidget);
    listWidget->addItem(report->name());
}
