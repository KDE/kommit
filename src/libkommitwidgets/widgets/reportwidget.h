#pragma once

#include "ui_reportwidget.h"

#include "libkommitwidgets_export.h"

class AbstractReport;
class LIBKOMMITWIDGETS_EXPORT ReportWidget : public QWidget, private Ui::ReportWidget
{
    Q_OBJECT

    AbstractReport *mReport;

public:
    explicit ReportWidget(AbstractReport *report, QWidget *parent = nullptr);
};
