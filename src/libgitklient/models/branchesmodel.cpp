/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesmodel.h"
#include "gitmanager.h"

#include <KLocalizedString>

#include <QDebug>

namespace Git
{

BranchesModel::BranchesModel(Manager *git, QObject *parent)
    : AbstractGitItemsModel{git, parent}
{
}

int BranchesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int BranchesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant BranchesModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    switch (index.column()) {
    case 0:
        return mData.at(index.row())->name;
    case 1:
        return mData.at(index.row())->commitsBehind;
    case 2:
        return mData.at(index.row())->commitsAhead;
    }

    return {};
}

QVariant BranchesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case 0:
        return i18n("Name");
    case 1:
        return i18n("Commit(s) behind");
    case 2:
        return i18n("Commit(s) ahead");
    }
    return {};
}

BranchesModel::BranchData *BranchesModel::fromIndex(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return nullptr;

    return mData.at(index.row());
}

void BranchesModel::fill()
{
    qDeleteAll(mData);
    mData.clear();

    QStringList branchesList;
    const auto out = mGit->readAllNonEmptyOutput({QStringLiteral("branch"), QStringLiteral("--list")});

    for (const auto &line : out) {
        auto b = line.trimmed();
        if (b.isEmpty())
            continue;
        if (b.startsWith(QStringLiteral("* "))) {
            b = b.mid(2);
            mReferenceBranch = mCurrentBranch = b.trimmed();
        }

        branchesList.append(b.trimmed());

        auto branch = new BranchData;
        branch->name = b.trimmed();
        branch->commitsAhead = branch->commitsBehind = 0;
        mData.append(branch);
    }
    calculateCommitStats();
}

const QString &BranchesModel::referenceBranch() const
{
    return mReferenceBranch;
}

void BranchesModel::calculateCommitStats()
{
    qDebug() << Q_FUNC_INFO;
    for (auto &b : mData) {
        const auto commitsInfo = mGit->uniqueCommiteOnBranches(mReferenceBranch, b->name);
        b->commitsBehind = commitsInfo.first;
        b->commitsAhead = commitsInfo.second;
    }
    Q_EMIT dataChanged(index(0, 1), index(mData.size() - 1, 2));
}

void BranchesModel::setReferenceBranch(const QString &newReferenceBranch)
{
    mReferenceBranch = newReferenceBranch;

    calculateCommitStats();
}

const QString &BranchesModel::currentBranch() const
{
    return mCurrentBranch;
}

} // namespace Git
