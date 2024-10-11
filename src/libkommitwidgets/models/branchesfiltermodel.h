/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QScopedPointer>
#include <QSortFilterProxyModel>

class TreeModel;
class BranchesFilterModelPrivate;
class BranchesFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    BranchesFilterModel(QObject *parent);

    Q_REQUIRED_RESULT TreeModel *treeModel() const;
    void setTreeModel(TreeModel *newTreeModel);

    Q_REQUIRED_RESULT QString filterTerm() const;
    void setFilterTerm(const QString &newFilterTerm);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QScopedPointer<BranchesFilterModelPrivate> d_ptr;
    Q_DECLARE_PRIVATE(BranchesFilterModel)
};
