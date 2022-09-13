/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

/*
Copyright (C) 2021 by Hamed Masafi <hamed.masafi@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// application headers
#include "appwindow.h"
#include "appdebug.h"
#include "dialogs/changedfilesdialog.h"
#include "dialogs/clonedialog.h"
#include "dialogs/commitpushdialog.h"
#include "dialogs/fetchdialog.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/initdialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/pulldialog.h"
#include "dialogs/reposettingsdialog.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/selectbranchestodiffdialog.h"
#include "dialogs/switchbranchdialog.h"
#include "diffwindow.h"
#include "git/commands/commandpull.h"
#include "git/commands/commandswitchbranch.h"
#include "git/gitmanager.h"
#include "git/models/logsmodel.h"
#include "multipagewidget.h"
#include "settings/settingsmanager.h"
#include "widgets/branchesstatuswidget.h"
#include "widgets/commitswidget.h"
#include "widgets/historyviewwidget.h"
#include "widgets/remoteswidget.h"
#include "widgets/stasheswidget.h"
#include "widgets/submoduleswidget.h"
#include "widgets/tagswidget.h"

// KF headers
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QSettings>
#include <QStatusBar>
#include <QtConcurrent/QtConcurrent>

void AppWindow::init()
{
    mGit = Git::Manager::instance();
    connect(mGit, &Git::Manager::pathChanged, this, &AppWindow::git_pathChanged);

    initActions();
    mMainWidget = new MultiPageWidget(this);
    mMainWidget->setDefaultGitManager(mGit);
    addPage<HistoryViewWidget>(QStringLiteral("view_overview"));
    addPage<BranchesStatusWidget>(QStringLiteral("view_branches"));
    addPage<CommitsWidget>(QStringLiteral("view_commits"));
    addPage<StashesWidget>(QStringLiteral("view_stashes"));
    addPage<SubmodulesWidget>(QStringLiteral("view_submodules"));
    addPage<RemotesWidget>(QStringLiteral("view_remotes"));
    addPage<TagsWidget>(QStringLiteral("view_tags"));

    setupGUI(StandardWindowOption::Default, "gitklientui.rc");
    mMainWidget->setCurrentIndex(0);

    setCentralWidget(mMainWidget);

    mStatusCurrentBranchLabel = new QLabel(statusBar());
    statusBar()->addPermanentWidget(mStatusCurrentBranchLabel);
    mStatusCurrentBranchLabel->setText(i18n("No repo selected"));
}

AppWindow::AppWindow()
    : AppMainWindow()
{
    init();

    if (GitKlientSettings::openLastRepo()) {
        QSettings s;
        auto p = s.value("last_repo").toString();
        mGit->setPath(p);
        initRecentFiles(p);
        QtConcurrent::run(this, &AppWindow::loadRemotes);
    }
}

AppWindow::AppWindow(const QString &path)
{
    init();
    mGit->setPath(path);
    QtConcurrent::run(this, &AppWindow::loadRemotes);
}

AppWindow::~AppWindow()
{
    QSettings s;
    for (auto &w : _baseWidgets)
        w->saveState(s);
}

AppWindow *AppWindow::instance()
{
    static auto *instance = new AppWindow;
    return instance;
}

void AppWindow::git_pathChanged()
{
    setWindowFilePath(mGit->path());
    //    setWindowTitle(_git->path());

    auto statusText = i18n("Current branch: %1", mGit->currentBranch());
    if (mGit->isMerging())
        statusText.append(i18n(" (merging)"));

    mStatusCurrentBranchLabel->setText(statusText);
}

void AppWindow::initActions()
{
    KActionCollection *actionCollection = this->actionCollection();

    auto repoInitAction = actionCollection->addAction("repo_init", this, &AppWindow::initRepo);
    repoInitAction->setText(i18n("Init..."));

    auto repoOpenAction = actionCollection->addAction("repo_open", this, &AppWindow::openRepo);
    repoOpenAction->setText(i18n("Open..."));
    actionCollection->setDefaultShortcut(repoOpenAction, QKeySequence("Ctrl+O"));
    repoOpenAction->setIcon(QIcon::fromTheme(QStringLiteral("folder-open")));

    auto repoCloneAction = actionCollection->addAction("repo_clone", this, &AppWindow::clone);
    repoCloneAction->setText(i18n("Clone..."));

    auto repoStatusAction = actionCollection->addAction(QStringLiteral("repo_status"), this, &AppWindow::repoStatus);
    repoStatusAction->setText(i18n("Changed files..."));
    repoStatusAction->setIcon(QIcon::fromTheme("gitklient-changedfiles"));
    actionCollection->setDefaultShortcut(repoStatusAction, QKeySequence("Ctrl+S"));

    {
        recentAction = actionCollection->addAction("recent");
        recentAction->setText(i18n("Recent repos"));
        recentAction->setMenu(new QMenu(this));
        initRecentFiles();
    }

    auto repoPullAction = actionCollection->addAction("repo_pull", this, &AppWindow::pull);
    repoPullAction->setText(i18n("Pull..."));
    repoPullAction->setIcon(QIcon::fromTheme("git-pull"));

    auto repoFetchAction = actionCollection->addAction("repo_fetch", this, &AppWindow::fetch);
    repoFetchAction->setText(i18n("Fetch..."));
    repoFetchAction->setIcon(QIcon::fromTheme("git-fetch"));

    auto repoPushAction = actionCollection->addAction("repo_push", this, &AppWindow::commitPushAction);
    repoPushAction->setText(i18n("Push..."));
    repoPushAction->setIcon(QIcon::fromTheme("git-push"));

    auto repoMergeAction = actionCollection->addAction("repo_merge", this, &AppWindow::merge);
    repoMergeAction->setText(i18n("Merge..."));
    repoMergeAction->setIcon(QIcon::fromTheme("git-merge"));

    auto diffBranchesAction = actionCollection->addAction("diff_branches", this, &AppWindow::diffBranches);
    diffBranchesAction->setText(i18n("Diff branches..."));

    auto repoSearchAction = actionCollection->addAction("repo_search", this, &AppWindow::search);
    repoSearchAction->setText(i18n("Search..."));

    auto repoSettingsAction = actionCollection->addAction("repo_settings", this, &AppWindow::repoSettings);
    repoSettingsAction->setText(i18n("Repo settings..."));

    auto repoSwitchAction = actionCollection->addAction("repo_switch", this, &AppWindow::repoSwitch);
    repoSwitchAction->setText(i18n("Switch/Checkout..."));

    auto repoDiffTreeAction = actionCollection->addAction("repo_diff_tree", this, &AppWindow::repoDiffTree);
    repoDiffTreeAction->setText(i18n("Diff tree"));

    KStandardAction::quit(this, &QMainWindow::close, actionCollection);
    KStandardAction::preferences(SettingsManager::instance(), &SettingsManager::show, actionCollection);
    //    KStandardAction::openNew(this, &GitKlientWindow::clone, actionCollection);
}
void AppWindow::initRecentFiles(const QString &newItem)
{
    recentAction->menu()->clear();
    QSettings s;
    auto recentList = s.value("recent_files").toStringList();
    if (!newItem.isEmpty()) {
        recentList.removeOne(newItem);
        recentList.prepend(newItem);
        s.setValue("recent_files", recentList);
        s.setValue("last_repo", newItem);
        s.sync();
    }
    for (const auto &item : recentList) {
        auto action = recentAction->menu()->addAction(item);
        action->setData(item);
        connect(action, &QAction::triggered, this, &AppWindow::recentActionTriggered);
    }
}

void AppWindow::loadRemotes()
{
    auto remotes = mGit->remotes();
    for (const auto &r : remotes)
        volatile auto remote = mGit->remoteDetails(r);
}

void AppWindow::repoStatus()
{
    ChangedFilesDialog d(mGit, this);
    d.exec();
}

void AppWindow::initRepo()
{
    InitDialog d(mGit, this);
    if (d.exec() == QDialog::Accepted) {
        QDir dir;
        if (!dir.mkpath(d.path())) {
            KMessageBox::error(this, i18n("Unable to create path: %1", d.path()), i18n("Init repo"));
            return;
        }
        mGit->init(d.path());
        mGit->setPath(d.path());
    }
}

void AppWindow::openRepo()
{
    QFileDialog d;
    d.setFileMode(QFileDialog::Directory);
    if (d.exec() == QDialog::Accepted) {
        mGit->setPath(d.directoryUrl().toLocalFile());
        //        m_kde_actionsView->reload();
        initRecentFiles(d.directoryUrl().toLocalFile());
    }
}

void AppWindow::recentActionTriggered()
{
    auto action = qobject_cast<QAction *>(sender());
    if (!action)
        return;

    auto p = action->data().toString();
    mGit->setPath(p);

    initRecentFiles(p);

    QtConcurrent::run(this, &AppWindow::loadRemotes);
}

void AppWindow::commitPushAction()
{
    CommitPushDialog d(mGit, this);
    if (d.exec() == QDialog::Accepted)
        mGit->logsModel()->load();
}

void AppWindow::pull()
{
    PullDialog d(this);
    d.exec();
}

void AppWindow::fetch()
{
    FetchDialog d(mGit, this);
    d.exec();
}

void AppWindow::showBranchesStatus()
{
    MergeDialog d(mGit, this);
    d.exec();
}

void AppWindow::clone()
{
    CloneDialog d(this);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r(this);

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
}

void AppWindow::diffBranches()
{
    SelectBranchesToDiffDialog d(mGit, this);
    if (d.exec() == QDialog::Accepted) {
        auto diffWin = new DiffWindow(mGit, d.oldBranch(), d.newBranch());
        diffWin->showModal();
    }
}

void AppWindow::search()
{
    SearchDialog d(mGit, this);
    d.exec();
}

void AppWindow::repoSettings()
{
    RepoSettingsDialog d(mGit, this);
    d.exec();
}

void AppWindow::repoSwitch()
{
    if (mGit->isMerging()) {
        KMessageBox::error(this, i18n("Cannot switch branch while merging"), i18n("Switch branch"));
        return;
    }
    SwitchBranchDialog d(mGit, this);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner(this);
        runner.run(d.command());
        runner.exec();
    }
}

void AppWindow::repoDiffTree()
{
    auto w = new DiffWindow(mGit);
    w->showModal();
}

void AppWindow::merge()
{
    MergeDialog d(mGit, this);
    d.exec();
}

template<class T>
void AppWindow::addPage(const QString &actionName)
{
    const QList<Qt::Key> keys = {Qt::Key_0, Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9};
    auto action = actionCollection()->addAction(actionName);
    auto w = new T(mGit, this);
    action->setText(w->windowTitle());
    action->setIcon(QIcon::fromTheme(actionName));
    if (mMainWidget->count() < 10)
        actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL + keys[mMainWidget->count()]));

    mMainWidget->addPage(w, action);
    QSettings s;
    w->restoreState(s);
    _baseWidgets.append(w);
}
