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

class CommitsFilterModel;
class GraphPainter;
class CommitActions;
class HistoryViewWidget : public WidgetBase, private Ui::HistoryViewWidget
{
    Q_OBJECT

public:
    explicit HistoryViewWidget(QWidget *parent = nullptr);
    explicit HistoryViewWidget(Git::Manager *git, AppWindow *parent = nullptr);
    void setBranch(const QString &branchName);

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private Q_SLOTS:
    void on_treeViewHistory_itemActivated(const QModelIndex &index);
    void on_textBrowser_hashClicked(const QString &hash);
    void on_textBrowser_fileClicked(const QString &file);
    void on_treeViewHistory_customContextMenuRequested(const QPoint &pos);

private:
    CommitActions *mActions{nullptr};
    Git::LogsModel *mHistoryModel{nullptr};
    CommitsFilterModel *filterModel{nullptr};
    GraphPainter *mGraphPainter{nullptr};
};
