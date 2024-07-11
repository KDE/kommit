/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesmodel.h"
#include "caches/branchescache.h"
#include "entities/branch.h"
#include "gitmanager.h"

#include <KLocalizedString>

class BranchesModelPrivate
{
    BranchesModel *q_ptr;
    Q_DECLARE_PUBLIC(BranchesModel)

public:
    BranchesModelPrivate(BranchesModel *parent);

    void calculateCommitStats();

    QMap<QSharedPointer<Git::Branch>, QPair<int, int>> mCompareWithRef;
    QList<QSharedPointer<Git::Branch>> mData;
    QString mCurrentBranch;
    QString mReferenceBranch;
};

BranchesModel::BranchesModel(Git::Manager *git, QObject *parent)
    : AbstractGitItemsModel{git, parent}
    , d_ptr{new BranchesModelPrivate{this}}
{
}

BranchesModel::~BranchesModel()
{
    Q_D(BranchesModel);
    delete d;
}

int BranchesModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const BranchesModel);
    Q_UNUSED(parent)
    return d->mData.size();
}

int BranchesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 7;
}

QVariant BranchesModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BranchesModel);

    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= d->mData.size())
        return {};

    switch (index.column()) {
    case 0:
        return d->mData.at(index.row())->name();
    case 1:
        return d->mCompareWithRef.value(d->mData.at(index.row())).first;
    case 2:
        return d->mCompareWithRef.value(d->mData.at(index.row())).second;
    case 3:
        return d->mData.at(index.row())->isHead();
    case 4:
        return d->mData.at(index.row())->refName(); // d->mData.at(index.row())->commitsBehind;
    case 5:
        return d->mData.at(index.row())->upStreamName(); // d->mData.at(index.row())->commitsAhead;
    case 6:
        return d->mData.at(index.row())->remoteName();
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
    case 3:
        return i18n("Is ahead");
    case 4:
        return i18n("Ref");
    case 5:
        return i18n("Upstream");
    case 6:
        return i18n("Remote");
    }
    return {};
}

QSharedPointer<Git::Branch> BranchesModel::fromIndex(const QModelIndex &index) const
{
    Q_D(const BranchesModel);

    if (!index.isValid() || index.row() < 0 || index.row() >= d->mData.size())
        return nullptr;

    return d->mData.at(index.row());
}

QSharedPointer<Git::Branch> BranchesModel::findByName(const QString &branchName) const
{
    Q_D(const BranchesModel);

    auto i = std::find_if(d->mData.begin(), d->mData.end(), [&branchName](QSharedPointer<Git::Branch> branch) {
        return branch->name() == branchName;
    });
    if (i == d->mData.end())
        return nullptr;

    return *i;
}

void BranchesModel::reload()
{
    Q_D(BranchesModel);

    d->mData.clear();

    d->mData = mGit->branches()->allBranches(Git::BranchType::AllBranches);
    d->calculateCommitStats();
}

const QString &BranchesModel::referenceBranch() const
{
    Q_D(const BranchesModel);
    return d->mReferenceBranch;
}

BranchesModelPrivate::BranchesModelPrivate(BranchesModel *parent)
    : q_ptr{parent}
{
}

void BranchesModelPrivate::calculateCommitStats()
{
    Q_Q(BranchesModel);
    for (auto &b : mData) {
        const auto commitsInfo = q->manager()->uniqueCommitsOnBranches(mReferenceBranch, b->name());
        mCompareWithRef[b] = commitsInfo;
    }
    Q_EMIT q->dataChanged(q->index(0, 1), q->index(mData.size() - 1, 2));
}

void BranchesModel::setReferenceBranch(const QString &newReferenceBranch)
{
    Q_D(BranchesModel);
    d->mReferenceBranch = newReferenceBranch;
    d->calculateCommitStats();
}

void BranchesModel::clear()
{
    Q_D(BranchesModel);
    beginResetModel();
    d->mData.clear();
    endResetModel();
}

const QString &BranchesModel::currentBranch() const
{
    Q_D(const BranchesModel);
    return d->mCurrentBranch;
}

#include "moc_branchesmodel.cpp"
