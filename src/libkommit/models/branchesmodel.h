/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractgititemsmodel.h"
#include "libkommit_export.h"
#include "types.h"

namespace Git
{

class Branch;
class Manager;
class LIBKOMMIT_EXPORT BranchesModel : public AbstractGitItemsModel
{
    Q_OBJECT

public:
    struct BranchData {
        QString name;
        int commitsAhead;
        int commitsBehind;
    };
    explicit BranchesModel(Manager *git, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QSharedPointer<Branch> fromIndex(const QModelIndex &index) const;
    QSharedPointer<Branch> findByName(const QString &branchName) const;

    Q_REQUIRED_RESULT const QString &currentBranch() const;
    Q_REQUIRED_RESULT const QString &referenceBranch() const;
    void setReferenceBranch(const QString &newReferenceBranch);

    void clear() override;

protected:
    void fill() override;

private:
    QMap<QSharedPointer<Branch>, QPair<int, int>> mCompareWithRef;
    PointerList<Branch> mData;
    QString mCurrentBranch;
    QString mReferenceBranch;
    void calculateCommitStats();
    void calc();
};

} // namespace Git
