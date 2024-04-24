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

private Q_SLOTS:
    void reloadReports();
    void mSlotToolButtonTableClicked();
    void mSlotToolButtonChartClicked();

private:
    void addReport(AbstractReport *report);
    QList<ReportWidget *> mReportWidgets;
};
