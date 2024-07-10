/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_tagswidget.h"
#include "widgetbase.h"

class TagsModel;
class TagsActions;
class TagsWidget : public WidgetBase, private Ui::TagsWidget
{
    Q_OBJECT

public:
    explicit TagsWidget(RepositoryData *git, AppWindow *parent = nullptr);
    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotTreeViewTagsCustomContextMenuRequested(const QPoint &pos);
    void slotTreeViewTagsItemActivated(const QModelIndex &index);
    TagsActions *const mActions;
    TagsModel *const mModel;
};
