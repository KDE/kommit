/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_commitswidget.h"
#include "widgetbase.h"

namespace Git
{
class LogsModel;
};

class BranchActions;
class CommitActions;
class TreeModel;
class CommitsFilterModel;
class CommitsWidget : public WidgetBase, private Ui::CommitsWidget
{
    Q_OBJECT

public:
    explicit CommitsWidget(QWidget *parent = nullptr);
    explicit CommitsWidget(Git::Manager *git, AppWindow *parent = nullptr);
    void reload() override;

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;

private Q_SLOTS:
    void on_treeViewRepo_itemActivated(const QModelIndex &index);
    void on_treeViewRepo_customContextMenuRequested(const QPoint &pos);
    void on_treeViewHistory_itemActivated(const QModelIndex &index);
    void on_textBrowser_hashClicked(const QString &hash);
    void on_textBrowser_fileClicked(const QString &file);
    void on_treeViewHistory_customContextMenuRequested(const QPoint &pos);

    void on_lineEditFilter_textChanged(const QString &arg1);

private:
    void init();
    TreeModel *mRepoModel{nullptr};
    BranchActions *mActions{nullptr};
    CommitActions *mCommitActions{nullptr};
    Git::LogsModel *mHistoryModel{nullptr};
    CommitsFilterModel *mFilterModel{nullptr};
    QString mMainBranch;
    void setBranch(const QString &branchName);
};
