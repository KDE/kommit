/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <windows/appmainwindow.h>

class MultiPageWidget;
class WidgetBase;
class QLabel;

class RepositoryData;
namespace Git
{
}
/**
 * This class serves as the main window for kommit.  It handles the
 * menus, toolbars and status bars.
 *
 * @short Main window class
 * @author Hamed Masafi <hamed.masafi@gmail.com>
 * @version 0.1
 */
class AppWindow : public AppMainWindow
{
    Q_OBJECT
public:
    AppWindow();
    explicit AppWindow(const QString &path);
    ~AppWindow() override;

    static AppWindow *instance();

private:
    void gitPathChanged();
    void gitCurrentBranchChanged();
    void settingsUpdated();
    void repoStatus();
    void initRepo();
    void openRepo();
    void commitPushAction();
    void pull();
    void fetch();
    void showBranchesStatus();
    void clone();
    void diffBranches();
    void search();
    void repoSettings();
    void repoSwitch();
    void repoDiffTree();
    void merge();
    void cleanup();
    void initActions();
    void changeLogs();
    void initRecentRepos(const QString &newItem = QString());

    template<class T>
    void addPage(const QString &actionName, const QString &iconName);
    int pagesCount{0};
    void init();
    void updateActions(bool enabled);

    RepositoryData *mGitData{nullptr};

    QAction *mRecentAction = nullptr;
    MultiPageWidget *mMainWidget = nullptr;
    QList<WidgetBase *> mBaseWidgets;
    QLabel *mStatusCurrentBranchLabel = nullptr;

    QAction *mRepoInitAction = nullptr;
    QAction *mRepoOpenAction = nullptr;
    QAction *mRepoCloneAction = nullptr;
    QAction *mRepoStatusAction = nullptr;
    QAction *mRepoCleanupAction = nullptr;
    QAction *mRepoPullAction = nullptr;
    QAction *mRepoFetchAction = nullptr;
    QAction *mRepoPushAction = nullptr;
    QAction *mRepoMergeAction = nullptr;
    QAction *mDiffBranchesAction = nullptr;
    QAction *mRepoSearchAction = nullptr;
    QAction *mRepoSettingsAction = nullptr;
    QAction *mRepoSwitchAction = nullptr;
    QAction *mRepoDiffTreeAction = nullptr;
    QAction *mChangeLogsAction = nullptr;
};
