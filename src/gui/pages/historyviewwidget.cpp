/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "historyviewwidget.h"
#include "actions/commitactions.h"
#include "models/commitsmodel.h"

#include <core/repositorydata.h>
#include <entities/blob.h>
#include <entities/commit.h>
#include <gitmanager.h>
#include <widgets/graphpainter.h>
#include <windows/diffwindow.h>

HistoryViewWidget::HistoryViewWidget(RepositoryData *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new CommitActions(git->manager(), this))
    , mHistoryModel(git->commitsModel())
    , mGraphPainter(new GraphPainter(mHistoryModel, this))
{
    setupUi(this);
    treeViewHistory->setModel(mHistoryModel);

    treeViewHistory->setItemDelegateForColumn(0, mGraphPainter);

    // textBrowser->setEnableCommitsLinks(true);

    connect(treeViewHistory, &TreeView::itemActivated, this, &HistoryViewWidget::slotTreeViewHistoryItemActivated);
    connect(widgetCommit, &CommitDetails::hashClicked, this, &HistoryViewWidget::slotTextBrowserHashClicked);
    connect(widgetCommit, &CommitDetails::fileClicked, this, &HistoryViewWidget::slotTextBrowserFileClicked);
    connect(treeViewHistory, &TreeView::customContextMenuRequested, this, &HistoryViewWidget::slotTreeViewHistoryCustomContextMenuRequested);
}

void HistoryViewWidget::setBranch(QSharedPointer<Git::Branch> branch)
{
    treeViewHistory->setItemDelegateForColumn(0, nullptr);
    mHistoryModel->setBranch(branch);
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

    widgetCommit->setCommit(log.data()); // TODO: passing raw pointer
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
    auto log = widgetCommit->commit();

    if (!log)
        return;
    // TODO: let user choose the parent if they're more than one

    QSharedPointer<Git::Blob> oldFile;

    if (log->parents().size()) {
        oldFile = Git::Blob::lookup(mGit->manager()->repoPtr(), log->parents().first(), file);
    };
    auto newFile = Git::Blob::lookup(mGit->manager()->repoPtr(), log->commitHash(), file);

    auto diffWin = new DiffWindow(oldFile, newFile);
    diffWin->showModal();
}

void HistoryViewWidget::slotTreeViewHistoryCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto commit = mHistoryModel->fromIndex(treeViewHistory->currentIndex());
    if (!commit)
        return;
    mActions->setCommit(commit);

    mActions->popup();
}

#include "moc_historyviewwidget.cpp"
