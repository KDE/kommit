#include "reportwidget.h"

#include "reports/abstractreport.h"

ReportWidget::ReportWidget(AbstractReport *report, QWidget *parent)
    : QWidget(parent)
    , mReport{report}
{
    setupUi(this);

    tableView->setModel(report);
}
