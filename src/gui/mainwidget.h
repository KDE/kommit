/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_mainwidget.h"

namespace Git
{
class Manager;
}
class TreeModel;
class ButtonsGroup;
class MainWidget : public QWidget, private Ui::MainWidget
{
    Q_OBJECT
    int _listIndex{-1};
    QStringList _branches;
    Git::Manager *_git{nullptr};
    TreeModel *_repoModel;
    ButtonsGroup *_pagesButtonsGroup;
    QMenu *_branchesMenu;

public:
    explicit MainWidget(QWidget *parent = nullptr);
    virtual ~MainWidget();

    Git::Manager *git() const;
    void setGit(Git::Manager *newGit);

private slots:
    void reload();
    void listButton_clicked();
    void pageButtons_clicked(int index);

    void slotTreeViewRepoActivated(const QModelIndex &index);
    void slotTreeViewRepoCustomContextMenuRequested(const QPoint &pos);

    void slotActionBrowseBranchTriggered();
    void slotActionDiffBranchTriggered();
    void slotPushButtonAddTagClicked();
};
