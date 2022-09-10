/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_commitswidget.h"
#include "widgetbase.h"

class BranchActions;
class TreeModel;
class CommitsWidget : public WidgetBase, private Ui::CommitsWidget
{
    Q_OBJECT


public:
    explicit CommitsWidget(QWidget *parent = nullptr);
    explicit CommitsWidget(Git::Manager *git, AppWindow *parent = nullptr);
    void reload() override;

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private slots:
    void on_treeViewRepo_itemActivated(const QModelIndex &index);
    void on_treeViewRepo_customContextMenuRequested(const QPoint &pos);

private:
    void init();
    TreeModel *mRepoModel = nullptr;
    BranchActions *mActions = nullptr;
    QString mMainBranch;
};

