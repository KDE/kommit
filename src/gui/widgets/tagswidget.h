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

public:
    explicit TagsWidget(Git::Manager *git, AppWindow *parent = nullptr);
    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotTreeViewTagsCustomContextMenuRequested(const QPoint &pos);
    void slotTreeViewTagsItemActivated(const QModelIndex &index);
    TagsActions *mActions = nullptr;
    Git::TagsModel *mModel = nullptr;
};
