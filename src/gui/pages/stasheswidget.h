/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_stasheswidget.h"
#include "widgetbase.h"
#include <entities/stash.h>

namespace Git
{
class StashesModel;
}
class StashActions;
class StashesWidget : public WidgetBase, private Ui::StashesWidget
{
    Q_OBJECT

public:
    explicit StashesWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotTreeViewCustomContextMenuRequested(const QPoint &pos);
    void slotTreeViewItemActivated(const QModelIndex &index);
    void init();
    StashActions *const mActions;
    Git::StashesModel *const mModel;
};
