/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_remoteswidget.h"
#include "widgetbase.h"

class RemotesActions;
namespace Git
{
class RemotesModel;
class Manager;
}
class RemotesWidget : public WidgetBase, private Ui::RemotesWidget
{
    Q_OBJECT
    Git::RemotesModel *mModel;
    RemotesActions *mActions;

public:
    explicit RemotesWidget(QWidget *parent = nullptr);
    explicit RemotesWidget(Git::Manager *git, AppWindow *parent = nullptr);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private Q_SLOTS:
    void on_listView_itemActivated(const QModelIndex &index);
    void on_listView_customContextMenuRequested(const QPoint &pos);

private:
    void init(Git::Manager *git);
};
