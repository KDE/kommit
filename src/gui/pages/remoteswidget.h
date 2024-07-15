/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_remoteswidget.h"
#include "widgetbase.h"

namespace Git
{
class Manager;
}
class RemotesActions;
class RemotesModel;
class RemotesWidget : public WidgetBase, private Ui::RemotesWidget
{
    Q_OBJECT

public:
    explicit RemotesWidget(RepositoryData *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotListViewItemActivated(const QModelIndex &index);
    void slotListViewCustomContextMenuRequested(const QPoint &pos);
    void init();
    RemotesModel *const mModel;
    RemotesActions *const mActions;
};
