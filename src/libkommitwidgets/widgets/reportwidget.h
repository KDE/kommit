/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_reportwidget.h"

#include "libkommitwidgets_export.h"

#ifdef QT_CHARTS_LIB

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
namespace QtCharts
{
class QValueAxis;
class QBarSeries;
class QBarCategoryAxis;
class QChart;
};

using namespace QtCharts;
#else
class QValueAxis;
class QBarSeries;
class QBarCategoryAxis;
class QChart;
#endif

#endif

class AbstractReport;
class LIBKOMMITWIDGETS_EXPORT ReportWidget : public QWidget, private Ui::ReportWidget
{
    Q_OBJECT

    AbstractReport *mReport;

public:
    enum ShowMode {
        Table = 0,
        Chart = 1,
    };
    explicit ReportWidget(AbstractReport *report, QWidget *parent = nullptr);

    [[nodiscard]] int showMode() const;
    void setShowMode(int showMode);

    void reload();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void initChart();
    LIBKOMMITWIDGETS_NO_EXPORT void fillChart();
    LIBKOMMITWIDGETS_NO_EXPORT void fillTableWidget();
    LIBKOMMITWIDGETS_NO_EXPORT void slotZoomIn();
    LIBKOMMITWIDGETS_NO_EXPORT void slotZoomOut();
    LIBKOMMITWIDGETS_NO_EXPORT void slotZoomResetShortcut();

#ifdef QT_CHARTS_LIB
    QValueAxis *mAxis = nullptr;
    QBarSeries *mBarSeries = nullptr;
    QBarCategoryAxis *mAxisX = nullptr;
    QChart *mChart = nullptr;
#endif
};
