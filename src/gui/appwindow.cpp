/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

// application headers
#include "appwindow.h"
#include "commands/commandclean.h"
#include "commands/commandmerge.h"
#include "commands/commandswitchbranch.h"
#include "dialogs/changedfilesdialog.h"
#include "dialogs/cleanupdialog.h"
#include "dialogs/clonedialog.h"
#include "dialogs/commitpushdialog.h"
#include "dialogs/fetchdialog.h"
#include "dialogs/initdialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/pulldialog.h"
#include "dialogs/reposettingsdialog.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/searchdialog.h"
#include "dialogs/selectbranchestodiffdialog.h"
#include "dialogs/switchbranchdialog.h"
#include "diffwindow.h"
#include "gitmanager.h"
#include "models/logsmodel.h"
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

    setupGUI(StandardWindowOption::Default, QStringLiteral("gitklientui.rc"));
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
        auto p = s.value(QStringLiteral("last_repo")).toString();
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
    for (auto &w : mBaseWidgets)
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

    auto repoInitAction = actionCollection->addAction(QStringLiteral("repo_init"), this, &AppWindow::initRepo);
    repoInitAction->setText(i18n("Init..."));

    auto repoOpenAction = actionCollection->addAction(QStringLiteral("repo_open"), this, &AppWindow::openRepo);
    repoOpenAction->setText(i18n("Open..."));
    actionCollection->setDefaultShortcut(repoOpenAction, QKeySequence(QStringLiteral("Ctrl+O")));
    repoOpenAction->setIcon(QIcon::fromTheme(QStringLiteral("folder-open")));

    auto repoCloneAction = actionCollection->addAction(QStringLiteral("repo_clone"), this, &AppWindow::clone);
    repoCloneAction->setText(i18n("Clone..."));

    auto repoStatusAction = actionCollection->addAction(QStringLiteral("repo_status"), this, &AppWindow::repoStatus);
    repoStatusAction->setText(i18n("Changed files..."));
    repoStatusAction->setIcon(QIcon::fromTheme(QStringLiteral("gitklient-changedfiles")));
    actionCollection->setDefaultShortcut(repoStatusAction, QKeySequence(QStringLiteral("Ctrl+S")));

    {
        mRecentAction = actionCollection->addAction(QStringLiteral("recent"));
        mRecentAction->setText(i18n("Recent repos"));
        mRecentAction->setMenu(new QMenu(this));
        initRecentFiles();
    }

    auto repoCleanupAction = actionCollection->addAction(QStringLiteral("repo_cleanup"), this, &AppWindow::cleanup);
    repoCleanupAction->setText(i18n("Cleanup..."));

    auto repoPullAction = actionCollection->addAction(QStringLiteral("repo_pull"), this, &AppWindow::pull);
    repoPullAction->setText(i18n("Pull..."));
    repoPullAction->setIcon(QIcon::fromTheme(QStringLiteral("git-pull")));

    auto repoFetchAction = actionCollection->addAction(QStringLiteral("repo_fetch"), this, &AppWindow::fetch);
    repoFetchAction->setText(i18n("Fetch..."));
    repoFetchAction->setIcon(QIcon::fromTheme(QStringLiteral("git-fetch")));

    auto repoPushAction = actionCollection->addAction(QStringLiteral("repo_push"), this, &AppWindow::commitPushAction);
    repoPushAction->setText(i18n("Push..."));
    repoPushAction->setIcon(QIcon::fromTheme(QStringLiteral("git-push")));

    auto repoMergeAction = actionCollection->addAction(QStringLiteral("repo_merge"), this, &AppWindow::merge);
    repoMergeAction->setText(i18n("Merge..."));
    repoMergeAction->setIcon(QIcon::fromTheme(QStringLiteral("git-merge")));

    auto diffBranchesAction = actionCollection->addAction(QStringLiteral("diff_branches"), this, &AppWindow::diffBranches);
    diffBranchesAction->setText(i18n("Diff branches..."));

    auto repoSearchAction = actionCollection->addAction(QStringLiteral("repo_search"), this, &AppWindow::search);
    repoSearchAction->setText(i18n("Search..."));

    auto repoSettingsAction = actionCollection->addAction(QStringLiteral("repo_settings"), this, &AppWindow::repoSettings);
    repoSettingsAction->setText(i18n("Repo settings..."));

    auto repoSwitchAction = actionCollection->addAction(QStringLiteral("repo_switch"), this, &AppWindow::repoSwitch);
    repoSwitchAction->setText(i18n("Switch/Checkout..."));

    auto repoDiffTreeAction = actionCollection->addAction(QStringLiteral("repo_diff_tree"), this, &AppWindow::repoDiffTree);
    repoDiffTreeAction->setText(i18n("Diff tree"));

    KStandardAction::quit(this, &QMainWindow::close, actionCollection);

    auto settingsManager = new SettingsManager(this);
    KStandardAction::preferences(settingsManager, &SettingsManager::show, actionCollection);
}
void AppWindow::initRecentFiles(const QString &newItem)
{
    mRecentAction->menu()->clear();
    QSettings s;
    auto recentList = s.value(QStringLiteral("recent_files")).toStringList();
    if (!newItem.isEmpty()) {
        recentList.removeOne(newItem);
        recentList.prepend(newItem);
        s.setValue(QStringLiteral("recent_files"), recentList);
        s.setValue(QStringLiteral("last_repo"), newItem);
        s.sync();
    }
    for (const auto &item : std::as_const(recentList)) {
        auto action = mRecentAction->menu()->addAction(item);
        action->setData(item);
        connect(action, &QAction::triggered, this, &AppWindow::recentActionTriggered);
    }
}

void AppWindow::loadRemotes()
{
    const auto remotes = mGit->remotes();
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
    auto dir = QFileDialog::getExistingDirectory(this, i18n("Open repository"));

    if (dir != QString()) {
        mGit->setPath(dir);
        initRecentFiles(dir);
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
    if (d.exec() == QDialog::Accepted)
        mGit->logsModel()->load();
}

void AppWindow::fetch()
{
    FetchDialog d(mGit, this);
    if (d.exec() == QDialog::Accepted)
        mGit->logsModel()->load();
}

void AppWindow::showBranchesStatus()
{
    MergeDialog d(mGit, this);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r(this);

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
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

void AppWindow::cleanup()
{
    CleanupDialog d;
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner;
        runner.run(d.command());
        runner.exec();
    }
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
    mBaseWidgets.append(w);
}
