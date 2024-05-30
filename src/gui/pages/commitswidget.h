/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_commitswidget.h"
#include "widgetbase.h"

namespace Git
{

class Branch;
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
    explicit CommitsWidget(Git::Manager *git, AppWindow *parent = nullptr);
    void reload() override;

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;
    void settingsUpdated() override;

private:
    void slotTreeViewRepoItemActivated(const QModelIndex &index);
    void slotTreeViewRepoCustomContextMenuRequested(const QPoint &pos);
    void slotTreeViewHistoryItemActivated(const QModelIndex &index);
    void slotTextBrowserHashClicked(const QString &hash);
    void slotTextBrowserFileClicked(const QString &file);
    void slotTreeViewHistoryCustomContextMenuRequested(const QPoint &pos);

    void slotLineEditFilterTextChanged(const QString &arg1);

    void init();
    QMap<QString, QSharedPointer<Git::Branch>> mBranchesMap;
    TreeModel *mRepoModel{nullptr};
    BranchActions *mActions{nullptr};
    CommitActions *mCommitActions{nullptr};
    Git::LogsModel *mHistoryModel{nullptr};
    CommitsFilterModel *mFilterModel{nullptr};
    QString mMainBranch;
    void setBranch(const QString &branchName);
};
