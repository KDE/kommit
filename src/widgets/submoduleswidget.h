/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_submoduleswidget.h"
#include "widgetbase.h"

class SubmoduleActions;

namespace Git {
class SubmodulesModel;
}

class SubmodulesWidget : public WidgetBase, private Ui::SubmodulesWidget
{
    Q_OBJECT
    SubmoduleActions *_actions;
    Git::SubmodulesModel *_model;

public:
    explicit SubmodulesWidget(QWidget *parent = nullptr);
    explicit SubmodulesWidget(Git::Manager *git, AppWindow *parent = nullptr);


    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

    void reload() override;
private slots:
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_treeView_activated(const QModelIndex &index);
};

