/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_reportwidget.h"

#include "libkommitwidgets_export.h"

#ifdef QT_CHARTS_LIB
namespace QtCharts
{
class QValueAxis;
class QBarSeries;
class QBarCategoryAxis;
}
#endif

class AbstractReport;
class LIBKOMMITWIDGETS_EXPORT ReportWidget : public QWidget, private Ui::ReportWidget
{
    Q_OBJECT

    AbstractReport *mReport;

public:
    enum ShowMode { Table, Chart };
    explicit ReportWidget(AbstractReport *report, QWidget *parent = nullptr);

    Q_REQUIRED_RESULT int showMode() const;
    void setShowMode(int showMode);

    void reload();

private:
    void initChart();
    void fillChart();
    void fillTableWidget();

#ifdef QT_CHARTS_LIB
    QtCharts::QValueAxis *axis;
    QtCharts::QBarSeries *barSeries;
    QtCharts::QBarCategoryAxis *axisX;
#endif
};
