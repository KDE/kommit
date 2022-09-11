/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_tagswidget.h"
#include "widgetbase.h"

namespace Git
{
class TagsModel;
}

class TagsActions;
class TagsWidget : public WidgetBase, private Ui::TagsWidget
{
    Q_OBJECT
    TagsActions *_actions;
    Git::TagsModel *_model;

public:
    explicit TagsWidget(QWidget *parent = nullptr);
    explicit TagsWidget(Git::Manager *git, AppWindow *parent = nullptr);

private Q_SLOTS:
    void on_treeViewTags_customContextMenuRequested(const QPoint &pos);
    void on_treeViewTags_itemActivated(const QModelIndex &index);

public:
    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;
};
