/*
SPDX-FileCopyrightText: 2022 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include "libkommitgui_private_export.h"
#include "ui_authorswidget.h"
#include "widgetbase.h"

class LIBKOMMITGUI_TESTS_EXPORT AuthorsWidget : public WidgetBase, private Ui::AuthorsWidget
{
    Q_OBJECT

public:
    explicit AuthorsWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;
};
