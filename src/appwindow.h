/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/


#pragma once

#include "core/appmainwindow.h"

#include "GitKlientSettings.h"

class MultiPageWidget;
class WidgetBase;
class QLabel;

namespace Git
{
class Manager;
}
/**
 * This class serves as the main window for gitklient.  It handles the
 * menus, toolbars and status bars.
 *
 * @short Main window class
 * @author Hamed Masafi <hamed.masafi@gmail.com>
 * @version 0.1
 */
class AppWindow : public AppMainWindow
{
    Q_OBJECT
    Git::Manager *mGit;

public:
    /**
     * Default Constructor
     */
    AppWindow();
    explicit AppWindow(const QString &path);

    /**
     * Default Destructor
     */
    ~AppWindow() override;

    static AppWindow *instance();

private Q_SLOTS:
    void git_pathChanged();
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

private:
    QAction *recentAction;
    MultiPageWidget *mMainWidget;
    QList<WidgetBase *> _baseWidgets;
    QLabel *mStatusCurrentBranchLabel;

    void initActions();
    void initRecentFiles(const QString &newItem = QString());
    void loadRemotes();

    template<class T>
    void addPage(const QString &actionName);
    void init();
};
