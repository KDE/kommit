/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "historyviewwidget.h"
#include "actions/commitactions.h"
#include "diffwindow.h"
#include "gitlog.h"
#include "gitmanager.h"
#include "models/logsmodel.h"
#include "widgets/graphpainter.h"

HistoryViewWidget::HistoryViewWidget(Git::Manager *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mHistoryModel(git->logsModel())
{
    setupUi(this);
    treeViewHistory->setModel(mHistoryModel);

    mGraphPainter = new GraphPainter(mHistoryModel, this);
    treeViewHistory->setItemDelegateForColumn(0, mGraphPainter);

    mActions = new CommitActions(git, this);
    textBrowser->setEnableCommitsLinks(true);

    connect(treeViewHistory, &TreeView::itemActivated, this, &HistoryViewWidget::slotTreeViewHistoryItemActivated);
    connect(textBrowser, &LogDetailsWidget::hashClicked, this, &HistoryViewWidget::slotTextBrowserHashClicked);
    connect(textBrowser, &LogDetailsWidget::fileClicked, this, &HistoryViewWidget::slotTextBrowserFileClicked);
    connect(treeViewHistory, &TreeView::customContextMenuRequested, this, &HistoryViewWidget::slotTreeViewHistoryCustomContextMenuRequested);

}

void HistoryViewWidget::setBranch(const QString &branchName)
{
    treeViewHistory->setItemDelegateForColumn(0, nullptr);
    mHistoryModel->setBranch(branchName);
    if (mHistoryModel->rowCount(QModelIndex()))
        treeViewHistory->setCurrentIndex(mHistoryModel->index(0));
}

void HistoryViewWidget::saveState(QSettings &settings) const
{
    save(settings, splitter);
}

void HistoryViewWidget::restoreState(QSettings &settings)
{
    restore(settings, splitter);
}

void HistoryViewWidget::slotTreeViewHistoryItemActivated(const QModelIndex &index)
{
    auto log = mHistoryModel->fromIndex(index);
    if (!log)
        return;

    textBrowser->setLog(log);
}

void HistoryViewWidget::slotTextBrowserHashClicked(const QString &hash)
{
    auto index = mHistoryModel->findIndexByHash(hash);
    if (index.isValid()) {
        treeViewHistory->setCurrentIndex(index);
        slotTreeViewHistoryItemActivated(index);
    }
}

void HistoryViewWidget::slotTextBrowserFileClicked(const QString &file)
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

void HistoryViewWidget::slotTreeViewHistoryCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto log = mHistoryModel->fromIndex(treeViewHistory->currentIndex());
    if (!log)
        return;
    mActions->setCommitHash(log->commitHash());

    mActions->popup();
}
