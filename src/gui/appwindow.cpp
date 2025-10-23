
/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

// application headers
#include "appwindow.h"
#include "changelogsdialog.h"
#include "commands/commandmerge.h"
#include "core/repositorydata.h"
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
#include "kommit_appdebug.h"
#include "models/commitsmodel.h"
#include "multipagewidget.h"
#include "pages/branchesstatuswidget.h"
#include "pages/commitswidget.h"
#include "pages/historyviewwidget.h"
#include "pages/remoteswidget.h"
#include "pages/reportswidget.h"
#include "pages/stasheswidget.h"
#include "pages/submoduleswidget.h"
#include "pages/tagswidget.h"
#include "settings/settingsmanager.h"

#include <Kommit/BranchesCache>
#include <Kommit/CommandClean>
#include <Kommit/CommandSwitchBranch>
#include <Kommit/Repository>

#include <KommitSettings.h>
#include <windows/diffwindow.h>

// KF headers
#include <KActionCollection>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardShortcut>
#include <QFileDialog>
#include <QMenu>
#include <QSettings>
#include <QStatusBar>

AppWindow::AppWindow()
    : AppMainWindow()
    , mGitData{new RepositoryData{Git::Repository::instance()}}
{
    init();
    QSettings s;

    if (KommitSettings::openLastRepo()) {
        const QString p = s.value(QStringLiteral("last_repo")).toString();

        if (!p.isEmpty()) {
            mGitData->manager()->open(p);
            initRecentRepos(p);
        }
    }

    auto lastDisplayedversion = s.value("last_displayed_version");

    if (lastDisplayedversion != qApp->applicationVersion()) {
        show();
        auto d = new ChangeLogsDialog{this};
        d->setParent(this);
        d->show();
        s.setValue("last_displayed_version", qApp->applicationVersion());
        s.sync();
    }
}

void AppWindow::init()
{
    connect(mGitData->manager(), &Git::Repository::pathChanged, this, &AppWindow::gitPathChanged);
    connect(mGitData->manager(), &Git::Repository::currentBranchChanged, this, &AppWindow::gitCurrentBranchChanged);

    initActions();

    mMainWidget = new MultiPageWidget{this};
    mMainWidget->setDefaultGitManager(mGitData->manager());
    addPage<HistoryViewWidget>(QStringLiteral("view_overview"), QStringLiteral("git_overview"));
    addPage<BranchesStatusWidget>(QStringLiteral("view_branches"), QStringLiteral("git_branch"));
    addPage<CommitsWidget>(QStringLiteral("view_commits"), QStringLiteral("git_commit"));
    addPage<StashesWidget>(QStringLiteral("view_stashes"), QStringLiteral("git_stash"));
    addPage<SubmodulesWidget>(QStringLiteral("view_submodules"), QStringLiteral("git_submodule"));
    addPage<RemotesWidget>(QStringLiteral("view_remotes"), QStringLiteral("git_remote"));
    addPage<TagsWidget>(QStringLiteral("view_tags"), QStringLiteral("git_tag"));
    addPage<ReportsWidget>(QStringLiteral("view_reports"), QStringLiteral("git_report"));

    setupGUI(StandardWindowOption::Default, QStringLiteral("kommitui.rc"));
    mMainWidget->setCurrentIndex(0);

    setCentralWidget(mMainWidget);

    mStatusCurrentBranchLabel = new QLabel(statusBar());
    statusBar()->addPermanentWidget(mStatusCurrentBranchLabel);
    mStatusCurrentBranchLabel->setText(i18n("No repo selected"));

    settingsUpdated();
    updateActions(false);
}

AppWindow::~AppWindow()
{
    QSettings s;
    for (auto &w : mBaseWidgets)
        w->saveState(s);
    qDeleteAll(mBaseWidgets);
}

AppWindow::AppWindow(const QString &path)
    : mGitData{new RepositoryData{Git::Repository::instance()}}
{
    init();
    mGitData->manager()->open(path);
}

AppWindow *AppWindow::instance()
{
    static auto *instance = new AppWindow;
    return instance;
}

void AppWindow::gitPathChanged()
{
    updateActions(mGitData->manager()->isValid());
    setWindowFilePath(mGitData->manager()->path());
}

void AppWindow::gitCurrentBranchChanged()
{
    if (mGitData->manager()->isValid()) {
        QString statusText = i18n("Current branch: %1", mGitData->manager()->branches()->currentName());
        if (mGitData->manager()->isMerging())
            statusText.append(i18n(" (merging)"));
        else if (mGitData->manager()->isRebasing())
            statusText.append(i18n(" (rebasing)"));

        mStatusCurrentBranchLabel->setText(statusText);
    } else {
        mStatusCurrentBranchLabel->setText(i18n("No repo selected"));
    }
}

void AppWindow::settingsUpdated()
{
    for (const auto &w : std::as_const(mBaseWidgets))
        w->settingsUpdated();
    mGitData->commitsModel()->setCalendarType(KommitSettings::calendarType());
}

void AppWindow::updateActions(bool enabled)
{
    mRepoStatusAction->setEnabled(enabled);
    mRepoCleanupAction->setEnabled(enabled);
    mRepoPullAction->setEnabled(enabled);
    mRepoFetchAction->setEnabled(enabled);
    mRepoPushAction->setEnabled(enabled);
    mRepoMergeAction->setEnabled(enabled);
    mDiffBranchesAction->setEnabled(enabled);
    mRepoSearchAction->setEnabled(enabled);
    mRepoSettingsAction->setEnabled(enabled);
    mRepoSwitchAction->setEnabled(enabled);
    mRepoDiffTreeAction->setEnabled(enabled);
}

void AppWindow::initActions()
{
    KActionCollection *actionCollection = this->actionCollection();

    mRepoInitAction = actionCollection->addAction(QStringLiteral("repo_init"), this, &AppWindow::initRepo);
    mRepoInitAction->setText(i18nc("@action", "Init…"));

    mRepoOpenAction = actionCollection->addAction(QStringLiteral("repo_open"), this, &AppWindow::openRepo);
    mRepoOpenAction->setText(i18nc("@action", "Open…"));
    actionCollection->setDefaultShortcuts(mRepoOpenAction, KStandardShortcut::open());
    mRepoOpenAction->setIcon(QIcon::fromTheme(QStringLiteral("folder-open")));

    mRepoCloneAction = actionCollection->addAction(QStringLiteral("repo_clone"), this, &AppWindow::clone);
    mRepoCloneAction->setText(i18nc("@action", "Clone…"));

    mRepoStatusAction = actionCollection->addAction(QStringLiteral("repo_status"), this, &AppWindow::repoStatus);
    mRepoStatusAction->setText(i18nc("@action", "Changed files…"));
    mRepoStatusAction->setIcon(QIcon::fromTheme(QStringLiteral("git_changedfiles")));
    actionCollection->setDefaultShortcut(mRepoStatusAction, QKeySequence(Qt::CTRL | Qt::Key_S));

    {
        mRecentAction = actionCollection->addAction(QStringLiteral("recent"));
        mRecentAction->setText(i18nc("@action", "Recent repos"));
        mRecentAction->setMenu(new QMenu(this));
        initRecentRepos();
    }

    mRepoCleanupAction = actionCollection->addAction(QStringLiteral("repo_cleanup"), this, &AppWindow::cleanup);
    mRepoCleanupAction->setText(i18nc("@action", "Cleanup…"));

    mRepoPullAction = actionCollection->addAction(QStringLiteral("repo_pull"), this, &AppWindow::pull);
    mRepoPullAction->setText(i18nc("@action", "Pull…"));
    mRepoPullAction->setIcon(QIcon::fromTheme(QStringLiteral("git_pull")));

    mRepoFetchAction = actionCollection->addAction(QStringLiteral("repo_fetch"), this, &AppWindow::fetch);
    mRepoFetchAction->setText(i18nc("@action", "Fetch…"));
    mRepoFetchAction->setIcon(QIcon::fromTheme(QStringLiteral("git_fetch")));

    mRepoPushAction = actionCollection->addAction(QStringLiteral("repo_push"), this, &AppWindow::commitPushAction);
    mRepoPushAction->setText(i18nc("@action", "Push…"));
    mRepoPushAction->setIcon(QIcon::fromTheme(QStringLiteral("git_push")));

    mRepoMergeAction = actionCollection->addAction(QStringLiteral("repo_merge"), this, &AppWindow::merge);
    mRepoMergeAction->setText(i18nc("@action", "Merge…"));
    mRepoMergeAction->setIcon(QIcon::fromTheme(QStringLiteral("git_merge")));

    mDiffBranchesAction = actionCollection->addAction(QStringLiteral("diff_branches"), this, &AppWindow::diffBranches);
    mDiffBranchesAction->setText(i18nc("@action", "Diff branches…"));

    mRepoSearchAction = actionCollection->addAction(QStringLiteral("repo_search"), this, &AppWindow::search);
    mRepoSearchAction->setText(i18nc("@action", "Search…"));

    mRepoSettingsAction = actionCollection->addAction(QStringLiteral("repo_settings"), this, &AppWindow::repoSettings);
    mRepoSettingsAction->setText(i18nc("@action", "Repo settings…"));

    mRepoSwitchAction = actionCollection->addAction(QStringLiteral("repo_switch"), this, &AppWindow::repoSwitch);
    mRepoSwitchAction->setText(i18nc("@action", "Switch/Checkout…"));

    mRepoDiffTreeAction = actionCollection->addAction(QStringLiteral("repo_diff_tree"), this, &AppWindow::repoDiffTree);
    mRepoDiffTreeAction->setText(i18nc("@action", "Diff tree…"));

    mChangeLogsAction = actionCollection->addAction(QStringLiteral("help_changelogs"), this, &AppWindow::changeLogs);
    mChangeLogsAction->setText(i18nc("@action", "Change logs…"));

    KStandardAction::quit(this, &QMainWindow::close, actionCollection);

    auto settingsManager = new SettingsManager(mGitData->manager(), this);
    connect(settingsManager, &SettingsManager::settingsUpdated, this, &AppWindow::settingsUpdated);
    KStandardAction::preferences(settingsManager, &SettingsManager::show, actionCollection);
}

void AppWindow::changeLogs()
{
    ChangeLogsDialog d{this};
    d.exec();
}
void AppWindow::initRecentRepos(const QString &newItem)
{
    mRecentAction->menu()->clear();
    QSettings s;
    auto recentList = s.value(QStringLiteral("recent_files")).toStringList();

    if (!newItem.isEmpty()) {
        recentList.removeOne(newItem);
        recentList.prepend(newItem);

        if (recentList.size() > 10)
            recentList = recentList.mid(0, 10);
        s.setValue(QStringLiteral("recent_files"), recentList);
        s.setValue(QStringLiteral("last_repo"), newItem);
        s.sync();
    }

    mRecentAction->setVisible(!recentList.isEmpty());

    int index{1};
    for (const auto &item : std::as_const(recentList)) {
        auto action = mRecentAction->menu()->addAction(QStringLiteral("%1    %2").arg(index++).arg(item));
        connect(action, &QAction::triggered, this, [this, item]() {
            if (mGitData->manager()->open(item)) {
                initRecentRepos(item);
            }
        });
    }
    mRecentAction->menu()->addSeparator();
    auto clearAction = mRecentAction->menu()->addAction(QIcon::fromTheme(QStringLiteral("edit-clear-history")), i18n("Clear"));
    connect(clearAction, &QAction::triggered, this, [this]() {
        QSettings s;
        s.setValue(QStringLiteral("recent_files"), QStringList());
        s.sync();
        initRecentRepos();
    });
}

void AppWindow::repoStatus()
{
    ChangedFilesDialog d(mGitData->manager(), this);
    d.exec();
}

void AppWindow::initRepo()
{
    InitDialog d(mGitData->manager(), this);
    if (d.exec() == QDialog::Accepted) {
        QDir dir;
        const QString path = d.path();
        if (!dir.mkpath(path)) {
            KMessageBox::error(this, i18n("Unable to create path: %1", path), i18n("Init repo"));
            return;
        }
        if (mGitData->manager()->init(path)) {
            mGitData->manager()->open(path);
        } else {
            qCWarning(KOMMIT_LOG) << " Impossible to initialize git in " << path;
        }
    }
}

void AppWindow::openRepo()
{
    const auto dir = QFileDialog::getExistingDirectory(this, i18n("Open repository"));

    if (!dir.isEmpty()) {
        mGitData->manager()->open(dir);
        initRecentRepos(dir);
    }
}

void AppWindow::commitPushAction()
{
    CommitPushDialog d(mGitData->manager(), this);
    // TODO: remove load
    if (d.exec() == QDialog::Accepted)
        mGitData->commitsModel()->load();
}

void AppWindow::pull()
{
    PullDialog d(mGitData->manager(), this);
    if (d.exec() == QDialog::Accepted)
        mGitData->commitsModel()->load();
}

void AppWindow::fetch()
{
    FetchDialog d(mGitData->manager(), this);
    if (d.exec() == QDialog::Accepted)
        mGitData->commitsModel()->load();
}

void AppWindow::showBranchesStatus()
{
    MergeDialog d(mGitData->manager(), this);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog r(mGitData->manager(), this);

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
        RunnerDialog r(mGitData->manager(), this);

        auto cmd = d.command();
        r.run(cmd);
        r.exec();
        cmd->deleteLater();
    }
}

void AppWindow::diffBranches()
{
    SelectBranchesToDiffDialog d(mGitData->manager(), this);
    if (d.exec() == QDialog::Accepted) {
        auto leftBranch = mGitData->manager()->branches()->findByName(d.oldBranch());
        auto rightBranch = mGitData->manager()->branches()->findByName(d.newBranch());

        auto diffWin = new DiffWindow(mGitData->manager(), &leftBranch, &rightBranch);
        diffWin->showModal();
    }
}

void AppWindow::search()
{
    SearchDialog d(mGitData->manager(), this);
    d.exec();
}

void AppWindow::repoSettings()
{
    RepoSettingsDialog d(mGitData->manager(), this);
    d.exec();
}

void AppWindow::repoSwitch()
{
    if (mGitData->manager()->isMerging()) {
        KMessageBox::error(this, i18n("Cannot switch branch while merging"), i18n("Switch branch"));
        return;
    }
    SwitchBranchDialog d(mGitData->manager(), this);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner(mGitData->manager(), this);
        runner.run(d.command());
        runner.exec();
    }
}

void AppWindow::repoDiffTree()
{
    auto currentBranch = mGitData->manager()->branches()->current();
    auto w = new DiffWindow(mGitData->manager(), &currentBranch);
    w->showModal();
}

void AppWindow::merge()
{
    MergeDialog d(mGitData->manager(), this);
    d.exec();
}

void AppWindow::cleanup()
{
    CleanupDialog d(this);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner(mGitData->manager());
        runner.run(d.command());
        runner.exec();
    }
}

template<class T>
void AppWindow::addPage(const QString &actionName, const QString &iconName)
{
    const QList<Qt::Key> keys = {Qt::Key_0, Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6, Qt::Key_7, Qt::Key_8, Qt::Key_9};
    auto action = actionCollection()->addAction(actionName);
    auto w = new T(mGitData, this);
    action->setText(w->windowTitle());
    if (pagesCount < 10)
        actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL | keys[pagesCount]));
    pagesCount++;
    auto icon = QIcon::fromTheme(iconName);

    w->setWindowIcon(icon);
    mMainWidget->addPage(w, action, icon); // action, icon);// w->windowIcon());
    QSettings s;
    w->restoreState(s);
    mBaseWidgets.append(w);
}

#include "moc_appwindow.cpp"
