/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesselectionwidget.h"
#include "models/branchesfiltermodel.h"
#include "models/treemodel.h"

#include <caches/branchescache.h>
#include <entities/branch.h>
#include <gitmanager.h>
#include <types.h>

class BranchesSelectionWidgetPrivate
{
    BranchesSelectionWidget *q_ptr;
    Q_DECLARE_PUBLIC(BranchesSelectionWidget)

public:
    BranchesSelectionWidgetPrivate(BranchesSelectionWidget *parent);

    TreeModel *treeModel{nullptr};
    BranchesFilterModel *filterModel;
    QMap<QString, QSharedPointer<Git::Branch>> branchesMap;
    Git::Manager *manager;

    void reload();
};

struct BranchTreeNode : public NodeData {
    QSharedPointer<Git::Branch> branch;
    BranchTreeNode(QSharedPointer<Git::Branch> branch);
};

BranchesSelectionWidget::BranchesSelectionWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr{new BranchesSelectionWidgetPrivate{this}}
{
    setupUi(this);

    Q_D(BranchesSelectionWidget);

    treeViewBranches->setModel(d->filterModel);

    connect(lineEditFilter, &QLineEdit::textChanged, d->filterModel, &BranchesFilterModel::setFilterTerm);
    connect(treeViewBranches, &QTreeView::clicked, this, &BranchesSelectionWidget::slotTreeViewRepoItemActivated);
    connect(treeViewBranches, &QTreeView::activated, this, &BranchesSelectionWidget::slotTreeViewRepoItemActivated);
    connect(comboBoxBranchesType, &QComboBox::currentIndexChanged, this, &BranchesSelectionWidget::reload);
}

Git::Manager *BranchesSelectionWidget::git() const
{
    Q_D(const BranchesSelectionWidget);
    return d->manager;
}

void BranchesSelectionWidget::setGit(Git::Manager *git)
{
    Q_D(BranchesSelectionWidget);
    d->manager = git;
    d->reload();
    connect(git, &Git::Manager::pathChanged, this, &BranchesSelectionWidget::reload);
}

void BranchesSelectionWidget::reload()
{
    Q_D(BranchesSelectionWidget);
    d->reload();
}

void BranchesSelectionWidget::slotTreeViewRepoItemActivated(const QModelIndex &index)
{
    Q_D(BranchesSelectionWidget);
    auto sourceIndex = d->filterModel->mapToSource(index);

    auto node = d->treeModel->node(sourceIndex);
    if (node->nodeData)
        Q_EMIT branchActivated(static_cast<BranchTreeNode *>(node->nodeData)->branch);
}

BranchesSelectionWidgetPrivate::BranchesSelectionWidgetPrivate(BranchesSelectionWidget *parent)
    : q_ptr{parent}
    , treeModel{new TreeModel{q_ptr}}
    , filterModel{new BranchesFilterModel{q_ptr}}
{
    filterModel->setTreeModel(treeModel);
}

void BranchesSelectionWidgetPrivate::reload()
{
    Q_Q(BranchesSelectionWidget);
    treeModel->clear();
    Git::BranchType t{Git::BranchType::AllBranches};

    switch (q->comboBoxBranchesType->currentIndex()) {
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

    const auto branches = manager->branches()->allBranches(t);
    for (const auto &branch : branches) {
        branchesMap.insert(branch->name(), branch);
        treeModel->addItem(branch->name(), new BranchTreeNode{branch});
    }
    treeModel->emitReset();
}

BranchTreeNode::BranchTreeNode(QSharedPointer<Git::Branch> branch)
    : branch{branch}
{
}

#include "moc_branchesselectionwidget.cpp"
