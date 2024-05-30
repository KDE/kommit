/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_reportswidget.h"
#include "widgetbase.h"

class AbstractReport;
class ReportWidget;
class ReportsWidget : public WidgetBase, private Ui::ReportsWidget
{
    Q_OBJECT

public:
    explicit ReportsWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void reloadReports();
    void slotToolButtonTableClicked();
    void slotToolButtonChartClicked();
    void addReport(AbstractReport *report);
    QList<ReportWidget *> mReportWidgets;
};
