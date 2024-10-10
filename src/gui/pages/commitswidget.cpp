/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitswidget.h"

#include "actions/branchactions.h"
#include "actions/commitactions.h"
#include "models/commitsfiltermodel.h"

#include <core/repositorydata.h>
#include <entities/blob.h>
#include <entities/branch.h>
#include <entities/commit.h>
#include <repository.h>
#include <models/commitsmodel.h>

#include <models/treemodel.h>
#include <windows/diffwindow.h>

#include <KommitSettings.h>

CommitsWidget::CommitsWidget(RepositoryData *git, AppWindow *parent)
    : WidgetBase(git, parent)
{
    setupUi(this);
    init();
}

void CommitsWidget::reload()
{
    // if (branchNames.contains(QStringLiteral("master")))
    //     mMainBranch = QStringLiteral("master");
    // else if (branchNames.contains(QStringLiteral("main")))
    //     mMainBranch = QStringLiteral("main");

    setBranch({});
}

void CommitsWidget::clear()
{
    mHistoryModel->clear();
}

void CommitsWidget::saveState(QSettings &settings) const
{
    save(settings, splitter);
    save(settings, splitter_2);
    save(settings, treeViewCommits);
}

void CommitsWidget::restoreState(QSettings &settings)
{
    restore(settings, splitter);
    restore(settings, splitter_2);
    restore(settings, treeViewCommits);
}

void CommitsWidget::settingsUpdated()
{
    mHistoryModel->setCalendarType(KommitSettings::calendarType());
}

void CommitsWidget::init()
{
    branchesView->setGit(mGit->manager());
    mHistoryModel = new CommitsModel(mGit->manager(), this);
    mHistoryModel->setCalendarType(KommitSettings::calendarType());
    mHistoryModel->setFullDetails(true);
    mFilterModel = new CommitsFilterModel(mHistoryModel, this);

    treeViewCommits->setModel(mFilterModel);

    mCommitActions = new CommitActions(mGit->manager(), this);

    connect(treeViewCommits, &TreeView::itemActivated, this, &CommitsWidget::slotTreeViewCommitsItemActivated);
    connect(treeViewCommits, &TreeView::itemActivated, this, &CommitsWidget::slotTreeViewCommitsItemActivated);
    connect(widgetCommitDetails, &CommitDetails::hashClicked, this, &CommitsWidget::slotTextBrowserHashClicked);
    connect(widgetCommitDetails, &CommitDetails::fileClicked, this, &CommitsWidget::slotTextBrowserFileClicked);
    connect(treeViewCommits, &QTreeView::customContextMenuRequested, this, &CommitsWidget::slotTreeViewCommitsCustomContextMenuRequested);
    connect(lineEditFilter, &QLineEdit::textChanged, this, &CommitsWidget::slotLineEditFilterTextChanged);
    connect(branchesView, &BranchesSelectionWidget::branchActivated, this, &CommitsWidget::setBranch);
}

void CommitsWidget::slotTreeViewCommitsItemActivated(const QModelIndex &index)
{
    auto commit = mHistoryModel->fromIndex(mFilterModel->mapToSource(index));
    if (!commit)
        return;

    widgetCommitDetails->setCommit(commit.data()); // TODO: passing raw pointer
}

void CommitsWidget::slotTextBrowserHashClicked(const QString &hash)
{
    const auto index = mHistoryModel->findIndexByHash(hash);
    if (index.isValid()) {
        treeViewCommits->setCurrentIndex(index);
        slotTreeViewCommitsItemActivated(index);
    }
}

void CommitsWidget::slotTextBrowserFileClicked(const QString &file)
{
    auto commit = widgetCommitDetails->commit();

    if (!commit || commit->parents().isEmpty())
        return;

    auto oldFile = Git::Blob::lookup(mGit->manager()->repoPtr(), commit->parents().constFirst(), file);
    auto newFile = Git::Blob::lookup(mGit->manager()->repoPtr(), commit->commitHash(), file);

    auto diffWin = new DiffWindow(oldFile, newFile);
    diffWin->showModal();
}

void CommitsWidget::slotTreeViewCommitsCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto log = mHistoryModel->fromIndex(treeViewCommits->currentIndex());
    if (!log)
        return;
    mCommitActions->setCommit(log);

    mCommitActions->popup();
}

void CommitsWidget::setBranch(QSharedPointer<Git::Branch> branch)
{
    treeViewCommits->setItemDelegateForColumn(0, nullptr);
    mHistoryModel->setBranch(branch);
    if (mHistoryModel->rowCount(QModelIndex()))
        treeViewCommits->setCurrentIndex(mHistoryModel->index(0));
}

void CommitsWidget::slotLineEditFilterTextChanged(const QString &text)
{
    mFilterModel->setFilterTerm(text);
}

#include "moc_commitswidget.cpp"
