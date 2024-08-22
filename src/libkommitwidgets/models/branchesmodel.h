/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"
#include "libkommitwidgets_export.h"

namespace Git
{

class Branch;
class Manager;
}
class BranchesModelPrivate;
class LIBKOMMITWIDGETS_EXPORT BranchesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    struct BranchData {
        QString name;
        int commitsAhead;
        int commitsBehind;
    };
    explicit BranchesModel(Git::Manager *git);
    ~BranchesModel();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QSharedPointer<Git::Branch> fromIndex(const QModelIndex &index) const;
    QSharedPointer<Git::Branch> findByName(const QString &branchName) const;

    [[nodiscard]] const QString &currentBranch() const;
    [[nodiscard]] const QString &referenceBranch() const;
    void setReferenceBranch(const QString &newReferenceBranch);

    void clear() override;
    void reload() override;

private:
    BranchesModelPrivate *d_ptr;
    Q_DECLARE_PRIVATE(BranchesModel)
};
