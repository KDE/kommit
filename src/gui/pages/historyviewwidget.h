/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_historyviewwidget.h"
#include "widgetbase.h"

namespace Git
{
class LogsModel;
};

class GraphPainter;
class CommitActions;
class HistoryViewWidget : public WidgetBase, private Ui::HistoryViewWidget
{
    Q_OBJECT

public:
    explicit HistoryViewWidget(Git::Manager *git, AppWindow *parent = nullptr);
    void setBranch(const QString &branchName);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private:
    void slotTreeViewHistoryItemActivated(const QModelIndex &index);
    void slotTextBrowserHashClicked(const QString &hash);
    void slotTextBrowserFileClicked(const QString &file);
    void slotTreeViewHistoryCustomContextMenuRequested(const QPoint &pos);
    CommitActions *const mActions;
    Git::LogsModel *const mHistoryModel;
    GraphPainter *mGraphPainter{nullptr};
};
