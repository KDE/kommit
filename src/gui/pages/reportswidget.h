#pragma once

#include "ui_reportswidget.h"
#include "widgetbase.h"

class AbstractReport;

class ReportsWidget : public WidgetBase, private Ui::ReportsWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private Q_SLOTS:
    void reloadReports();

private:
    void addReport(AbstractReport *report);
    QList<AbstractReport *> mReports;
};
