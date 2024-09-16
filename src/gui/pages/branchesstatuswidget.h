/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_branchesstatuswidget.h"
#include "widgetbase.h"

namespace Git
{
}
class BranchesModel;
class BranchActions;
class BranchesStatusWidget : public WidgetBase, private Ui::BranchesStatusWidget
{
    Q_OBJECT

public:
    explicit BranchesStatusWidget(RepositoryData *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void init();
    void slotTreeViewCustomContextMenuRequested(const QPoint &pos);
    void slotTreeViewActivated(const QModelIndex &index);
    void slotPushButtonRemoveSelectedClicked();
    void slotComboBoxReferenceBranchCurrentIndexChanged(int selectedBranch);
    void slotReloadData();

    BranchActions *const mActions;
    BranchesModel *const mModel;
};
