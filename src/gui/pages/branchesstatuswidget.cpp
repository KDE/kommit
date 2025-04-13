/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesstatuswidget.h"

#include "actions/branchactions.h"
#include "core/kmessageboxhelper.h"
#include "models/branchesmodel.h"

#include <core/repositorydata.h>

#include <Kommit/Branch>
#include <Kommit/BranchesCache>
#include <Kommit/Repository>

#include <KLocalizedString>
#include <KMessageBox>

BranchesStatusWidget::BranchesStatusWidget(RepositoryData *git, AppWindow *parent)
    : WidgetBase(git, parent)
    , mActions(new BranchActions(git->manager(), this))
    , mModel(git->branchesModel())

{
    setupUi(this);
    init();
}

void BranchesStatusWidget::init()
{
    treeView->setModel(mModel);

    comboBoxReferenceBranch->setModel(mModel);

    pushButtonNew->setAction(mActions->actionCreate());
    pushButtonBrowse->setAction(mActions->actionBrowse());
    pushButtonCheckout->setAction(mActions->actionCheckout());
    pushButtonDiff->setAction(mActions->actionDiff());
    pushButtonRemoveSelected->setAction(mActions->actionRemove());

    mActions->setOtherBranch(mModel->findByName(comboBoxReferenceBranch->currentText()));

    connect(comboBoxReferenceBranch, &QComboBox::currentIndexChanged, this, &BranchesStatusWidget::slotComboBoxReferenceBranchCurrentIndexChanged);
    connect(treeView, &QTreeView::customContextMenuRequested, this, &BranchesStatusWidget::slotTreeViewCustomContextMenuRequested);
    connect(treeView, &QTreeView::activated, this, &BranchesStatusWidget::slotTreeViewActivated);
    connect(treeView, &QTreeView::clicked, this, &BranchesStatusWidget::slotTreeViewActivated);
    connect(comboBoxBranchesType, &QComboBox::currentIndexChanged, this, &BranchesStatusWidget::slotReloadData);
}

void BranchesStatusWidget::saveState(QSettings &settings) const
{
    save(settings, treeView);
}

void BranchesStatusWidget::restoreState(QSettings &settings)
{
    restore(settings, treeView);
}

void BranchesStatusWidget::slotComboBoxReferenceBranchCurrentIndexChanged(int)
{
    const auto selectedBranch = comboBoxReferenceBranch->currentText();
    mModel->setReferenceBranch(selectedBranch);
    mActions->setOtherBranch(mModel->findByName(selectedBranch));
}

void BranchesStatusWidget::slotReloadData()
{
    Git::BranchType t{Git::BranchType::AllBranches};

    switch (comboBoxBranchesType->currentIndex()) {
    case 0:
        t = Git::BranchType::AllBranches;
        break;
    case 1:
        t = Git::BranchType::LocalBranch;
        break;
    case 2:
        t = Git::BranchType::RemoteBranch;
        break;
    }

    mModel->setBranchesType(t);
}

void BranchesStatusWidget::slotTreeViewCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    auto b = mModel->fromIndex(treeView->currentIndex());
    if (b.isNull())
        return;
    mActions->setBranch(b);
    mActions->popup();
}

void BranchesStatusWidget::slotTreeViewActivated(const QModelIndex &index)
{
    auto b = mModel->fromIndex(index);
    if (b.isNull())
        return;
    mActions->setBranch(b);
}

#include "moc_branchesstatuswidget.cpp"
