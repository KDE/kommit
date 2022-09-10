/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_stasheswidget.h"
#include "widgetbase.h"
#include "git/stash.h"

namespace Git {
class StashesModel;
}
class StashActions;
class StashesWidget : public WidgetBase, private Ui::StashesWidget
{
    Q_OBJECT
    StashActions *_actions;
    Git::StashesModel *_model;

public:
    explicit StashesWidget(QWidget *parent = nullptr);
    explicit StashesWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private slots:
    void on_treeView_customContextMenuRequested(const QPoint &pos);
    void on_treeView_itemActivated(const QModelIndex &index);

private:
    void init(Git::Manager *git);
};

