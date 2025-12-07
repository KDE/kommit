/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesfiltermodel.h"

#include "qdebug.h"
#include "treemodel.h"

class BranchesFilterModelPrivate
{
    BranchesFilterModel *q_ptr;
    Q_DECLARE_PUBLIC(BranchesFilterModel)

public:
    BranchesFilterModelPrivate(BranchesFilterModel *parent);

    TreeModel *treeModel{nullptr};
    QString filterTerm;
    bool acceptRow(const QModelIndex &index) const;
};
BranchesFilterModelPrivate::BranchesFilterModelPrivate(BranchesFilterModel *parent)
    : q_ptr{parent}
{
}

BranchesFilterModel::BranchesFilterModel(QObject *parent)
    : QSortFilterProxyModel{parent}
    , d_ptr{new BranchesFilterModelPrivate{this}}
{
}

BranchesFilterModel::~BranchesFilterModel()
{
}

TreeModel *BranchesFilterModel::treeModel() const
{
    Q_D(const BranchesFilterModel);
    return d->treeModel;
}

void BranchesFilterModel::setTreeModel(TreeModel *newTreeModel)
{
    Q_D(BranchesFilterModel);

    // beginResetModel();
    d->treeModel = newTreeModel;
    setSourceModel(newTreeModel);
    // endResetModel();
}

QString BranchesFilterModel::filterTerm() const
{
    Q_D(const BranchesFilterModel);

    return d->filterTerm;
}

void BranchesFilterModel::setFilterTerm(const QString &newFilterTerm)
{
    Q_D(BranchesFilterModel);

    d->filterTerm = newFilterTerm;
    invalidateFilter();
}

bool BranchesFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_D(const BranchesFilterModel);
    if (d->filterTerm.isEmpty()) {
        return true;
    }

    QModelIndex index = d->treeModel->index(sourceRow, 0, sourceParent);
    return d->acceptRow(index);
}

bool BranchesFilterModelPrivate::acceptRow(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return false;
    }

    QString data = treeModel->data(index, Qt::DisplayRole).toString();
    if (data.contains(filterTerm, Qt::CaseInsensitive)) {
        return true;
    }

    // Check child nodes
    int childCount = treeModel->rowCount(index);
    for (int i = 0; i < childCount; ++i) {
        if (acceptRow(treeModel->index(i, 0, index))) {
            return true;
        }
    }

    return false;
}

#include "moc_branchesfiltermodel.cpp"
