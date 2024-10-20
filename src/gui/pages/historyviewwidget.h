/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_historyviewwidget.h"
#include "widgetbase.h"

namespace Git
{
class Branch;
}

class CommitsModel;
class GraphPainter;
class CommitActions;
class HistoryViewWidget : public WidgetBase, private Ui::HistoryViewWidget
{
    Q_OBJECT

public:
    explicit HistoryViewWidget(RepositoryData *git, AppWindow *parent = nullptr);
    void setBranch(const Git::Branch &branch);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotTreeViewHistoryItemActivated(const QModelIndex &index);
    void slotTextBrowserHashClicked(const QString &hash);
    void slotTextBrowserFileClicked(const QString &file);
    void slotTreeViewHistoryCustomContextMenuRequested(const QPoint &pos);
    CommitActions *const mActions;
    CommitsModel *const mHistoryModel;
    GraphPainter *const mGraphPainter;
};
