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
};

class CommitsModel;
class CommitActions;
class CommitsFilterModel;
class CommitsWidget : public WidgetBase, private Ui::CommitsWidget
{
    Q_OBJECT

public:
    explicit CommitsWidget(RepositoryData *git, AppWindow *parent = nullptr);
    void reload() override;
    void clear() override;

    void saveState(QSettings &settings) const override;
    void restoreState(QSettings &settings) override;
    void settingsUpdated() override;

private:
    void slotTreeViewCommitsItemActivated(const QModelIndex &index);
    void slotTreeViewCommitsCustomContextMenuRequested(const QPoint &pos);
    void slotTextBrowserHashClicked(const QString &hash);
    void slotTextBrowserFileClicked(const QString &file);
    void slotLineEditFilterTextChanged(const QString &arg1);

    void setBranch(const Git::Branch &branch);
    void init();

    CommitActions *mCommitActions{nullptr};
    CommitsModel *mHistoryModel{nullptr};
    CommitsFilterModel *mFilterModel{nullptr};
    QString mMainBranch;
};
