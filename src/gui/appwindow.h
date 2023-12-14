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
class Manager;
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
    Git::Manager *mGit = nullptr;
    RepositoryData *mGitData{nullptr};

public:
    AppWindow();
    explicit AppWindow(const QString &path);
    ~AppWindow() override;

    static AppWindow *instance();

private Q_SLOTS:
    void git_pathChanged();
    void settingsUpdated();
    void repoStatus();
    void initRepo();
    void openRepo();
    void recentActionTriggered();
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

private:
    QAction *mRecentAction = nullptr;
    MultiPageWidget *mMainWidget = nullptr;
    QList<WidgetBase *> mBaseWidgets;
    QLabel *mStatusCurrentBranchLabel = nullptr;

    void initActions();
    void initRecentFiles(const QString &newItem = QString());

    template<class T>
    void addPage(const QString &actionName);
    void init();
};
