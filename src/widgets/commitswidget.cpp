/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitswidget.h"

#include "actions/branchactions.h"
#include "actions/commitactions.h"
#include "core/commitsfiltermodel.h"
#include "dialogs/filestreedialog.h"
#include "diffwindow.h"
#include "gitlog.h"
#include "gitmanager.h"
#include "models/logsmodel.h"
#include "models/treemodel.h"

CommitsWidget::CommitsWidget(QWidget *parent)
    : WidgetBase(parent)
{
    setupUi(this);
    init();
}

CommitsWidget::CommitsWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    init();
}

void CommitsWidget::reload()
{
    mRepoModel->clear();
    const auto branches = git()->branches();
    mRepoModel->addData(branches);

    if (branches.contains(QStringLiteral("master")))
        mMainBranch = QStringLiteral("master");
    else if (branches.contains(QStringLiteral("main")))
        mMainBranch = QStringLiteral("main");

    setBranch(QString());
}

void CommitsWidget::saveState(QSettings &settings) const
{
    save(settings, splitter);
    save(settings, splitter_2);
    save(settings, treeViewRepo);
    save(settings, treeViewHistory);
}

void CommitsWidget::restoreState(QSettings &settings)
{
    restore(settings, splitter);
    restore(settings, splitter_2);
    restore(settings, treeViewRepo);
    restore(settings, treeViewHistory);
}

void CommitsWidget::on_treeViewRepo_itemActivated(const QModelIndex &index)
{
    auto key = mRepoModel->key(index);
    if (!key.isEmpty())
        setBranch(key);
}

void CommitsWidget::on_treeViewRepo_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto branchName = mRepoModel->fullPath(treeViewRepo->currentIndex());
    mActions->setBranchName(branchName);
    mActions->popup();
}

void CommitsWidget::init()
{
    mHistoryModel = new Git::LogsModel(mGit, this);
    mHistoryModel->setFullDetails(true);
    mFilterModel = new CommitsFilterModel(mHistoryModel, this);

    treeViewHistory->setModel(mFilterModel);

    mRepoModel = new TreeModel(this);
    treeViewRepo->setModel(mRepoModel);

    mActions = new BranchActions(mGit, this);
    mCommitActions = new CommitActions(mGit, this);
}

void CommitsWidget::on_treeViewHistory_itemActivated(const QModelIndex &index)
{
    auto log = mHistoryModel->fromIndex(index);
    if (!log)
        return;

    textBrowser->setLog(log);
}

void CommitsWidget::on_textBrowser_hashClicked(const QString &hash)
{
    auto index = mHistoryModel->findIndexByHash(hash);
    if (index.isValid()) {
        treeViewHistory->setCurrentIndex(index);
        on_treeViewHistory_itemActivated(index);
    }
}

void CommitsWidget::on_textBrowser_fileClicked(const QString &file)
{
    auto log = textBrowser->log();

    Git::File oldFile;
    const Git::File newFile(log->commitHash(), file);
    if (!log->parents().empty()) {
        oldFile = {log->parents().first(), file};
    }
    auto diffWin = new DiffWindow(oldFile, newFile);
    diffWin->showModal();
}

void CommitsWidget::on_treeViewHistory_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto log = mHistoryModel->fromIndex(treeViewHistory->currentIndex());
    if (!log)
        return;
    mCommitActions->setCommitHash(log->commitHash());

    mActions->popup();
}

void CommitsWidget::setBranch(const QString &branchName)
{
    treeViewHistory->setItemDelegateForColumn(0, nullptr);
    mHistoryModel->setBranch(branchName);
    if (mHistoryModel->rowCount(QModelIndex()))
        treeViewHistory->setCurrentIndex(mHistoryModel->index(0));
}

void CommitsWidget::on_lineEditFilter_textChanged(const QString &text)
{
    mFilterModel->setFilterTerm(text);
}
