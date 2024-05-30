/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_submoduleswidget.h"
#include "widgetbase.h"

class SubmoduleActions;

namespace Git
{
class SubmodulesModel;
}

class SubmodulesWidget : public WidgetBase, private Ui::SubmodulesWidget
{
    Q_OBJECT

public:
    explicit SubmodulesWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotTreeViewCustomContextMenuRequested(const QPoint &pos);
    void slotTreeViewActivated(const QModelIndex &index);
    SubmoduleActions *const mActions;
    Git::SubmodulesModel *mModel = nullptr;
};
