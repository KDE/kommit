/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesmodel.h"
#include "caches/branchescache.h"
#include "entities/branch.h"
#include "repository.h"

#include <KLocalizedString>

class BranchesModelPrivate
{
    BranchesModel *q_ptr;
    Q_DECLARE_PUBLIC(BranchesModel)

public:
    BranchesModelPrivate(BranchesModel *parent);

    void calculateCommitStats();

    QMap<QSharedPointer<Git::Branch>, QPair<int, int>> compareWithRef;
    QList<QSharedPointer<Git::Branch>> data;
    QString currentBranch;
    QString referenceBranch;
    Git::BranchType branchType{Git::BranchType::AllBranches};
};

BranchesModel::BranchesModel(Git::Repository *git)
    : AbstractGitItemsModel{git}
    , d_ptr{new BranchesModelPrivate{this}}
{
    connect(git->branches(), &Git::BranchesCache::reseted, this, &BranchesModel::reload);
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
    return d->data.size();
}

int BranchesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 7;
}

QVariant BranchesModel::data(const QModelIndex &index, int role) const
{
    Q_D(const BranchesModel);

    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= d->data.size())
        return {};

    switch (index.column()) {
    case 0:
        return d->data.at(index.row())->name();
    case 1:
        return d->compareWithRef.value(d->data.at(index.row())).first;
    case 2:
        return d->compareWithRef.value(d->data.at(index.row())).second;
    case 3:
        return d->data.at(index.row())->isHead();
    case 4:
        return d->data.at(index.row())->refName();
    case 5:
        return d->data.at(index.row())->upStreamName();
    case 6:
        return d->data.at(index.row())->remoteName();
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

    if (!index.isValid() || index.row() < 0 || index.row() >= d->data.size())
        return nullptr;

    return d->data.at(index.row());
}

QSharedPointer<Git::Branch> BranchesModel::findByName(const QString &branchName) const
{
    Q_D(const BranchesModel);

    auto i = std::find_if(d->data.begin(), d->data.end(), [&branchName](QSharedPointer<Git::Branch> branch) {
        return branch->name() == branchName;
    });
    if (i == d->data.end())
        return nullptr;

    return *i;
}

void BranchesModel::reload()
{
    Q_D(BranchesModel);

    if (mGit->isValid()) {
        d->data = mGit->branches()->allBranches(d->branchType);
        d->calculateCommitStats();
    } else {
        d->data.clear();
        d->compareWithRef.clear();
    }
}

Git::BranchType BranchesModel::branchesType() const
{
    Q_D(const BranchesModel);
    return d->branchType;
}

void BranchesModel::setBranchesType(const Git::BranchType &newBranchType)
{
    Q_D(BranchesModel);
    d->branchType = newBranchType;
    load();
}

const QString &BranchesModel::referenceBranch() const
{
    Q_D(const BranchesModel);
    return d->referenceBranch;
}

BranchesModelPrivate::BranchesModelPrivate(BranchesModel *parent)
    : q_ptr{parent}
{
}

void BranchesModelPrivate::calculateCommitStats()
{
    Q_Q(BranchesModel);
    for (auto &b : data) {
        const auto commitsInfo = q->manager()->uniqueCommitsOnBranches(referenceBranch, b->name());
        compareWithRef[b] = commitsInfo;
    }
    Q_EMIT q->dataChanged(q->index(0, 1), q->index(data.size() - 1, 2));
}

void BranchesModel::setReferenceBranch(const QString &newReferenceBranch)
{
    Q_D(BranchesModel);
    d->referenceBranch = newReferenceBranch;
    d->calculateCommitStats();
}

void BranchesModel::clear()
{
    Q_D(BranchesModel);
    beginResetModel();
    d->data.clear();
    endResetModel();
}

const QString &BranchesModel::currentBranch() const
{
    Q_D(const BranchesModel);
    return d->currentBranch;
}

#include "moc_branchesmodel.cpp"
