/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submodulesmodel.h"
#include "caches/submodulescache.h"
#include "entities/submodule.h"
#include "repository.h"
#include <KLocalizedString>

namespace
{

QStringList statusTexts(const Git::Submodule &module)
{
    auto status = module.status();
    QStringList list;

    if (status & Git::Submodule::Status::InHead)
        list << QStringLiteral("superproject head contains submodule");
    if (status & Git::Submodule::Status::InIndex)
        list << QStringLiteral("superproject index contains submodule");
    if (status & Git::Submodule::Status::InConfig)
        list << QStringLiteral("superproject gitmodules has submodule");
    if (status & Git::Submodule::Status::InWd)
        list << QStringLiteral("superproject workdir has submodule");
    if (status & Git::Submodule::Status::IndexAdded)
        list << QStringLiteral("in index, not in head");
    if (status & Git::Submodule::Status::IndexDeleted)
        list << QStringLiteral("in head, not in index");
    if (status & Git::Submodule::Status::IndexModified)
        list << QStringLiteral("index and head don't match");
    if (status & Git::Submodule::Status::WdUninitialized)
        list << QStringLiteral("workdir contains empty directory");
    if (status & Git::Submodule::Status::WdAdded)
        list << QStringLiteral("in workdir, not index");
    if (status & Git::Submodule::Status::WdDeleted)
        list << QStringLiteral("in index, not workdir");
    if (status & Git::Submodule::Status::WdModified)
        list << QStringLiteral("index and workdir head don't match");
    if (status & Git::Submodule::Status::WdIndexModified)
        list << QStringLiteral("submodule workdir index is dirty");
    if (status & Git::Submodule::Status::WdWdModified)
        list << QStringLiteral("submodule workdir has modified files");
    if (status & Git::Submodule::Status::WdUntracked)
        list << QStringLiteral("wd contains untracked files");
    return list;
}

}

class SubmodulesModelPrivate
{
public:
    explicit SubmodulesModelPrivate(SubmodulesModel *parent, Git::Repository *manager);
    QList<Git::Submodule> list;
    Git::Repository *manager;
    Git::SubmodulesCache *cache;

    SubmodulesModel *q_ptr;
    Q_DECLARE_PUBLIC(SubmodulesModel)
};

SubmodulesModel::SubmodulesModel(Git::Repository *manager)
    : AbstractGitItemsModel{manager}
    , d_ptr{new SubmodulesModelPrivate{this, manager}}
{
    connect(manager->submodules(), &Git::SubmodulesCache::added, this, &SubmodulesModel::append);
}

SubmodulesModel::~SubmodulesModel()
{
}

int SubmodulesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    Q_D(const SubmodulesModel);
    return d->list.size();
}

int SubmodulesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

QVariant SubmodulesModel::data(const QModelIndex &index, int role) const
{
    Q_D(const SubmodulesModel);

    if (!index.isValid() || index.row() < 0 || index.row() >= d->list.size())
        return {};

    auto submodule = d->list.at(index.row());

    if (role == Qt::ToolTipRole) {
        return statusTexts(submodule).join(QLatin1Char('\n'));
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return submodule.path();
        case 1:
            return submodule.branch();
        case 2:
            return statusTexts(submodule).join(QStringLiteral(", "));
            // submodule->hasModifiedFiles() ? i18n("Modified") : QLatin1String();
        }
    }
    return {};
}

QVariant SubmodulesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case 0:
        return i18n("Path");
    case 1:
        return i18n("Head");
    case 2:
        return i18n("Status");
    }

    return {};
}

bool SubmodulesModel::append(const Git::Submodule &module)
{
    Q_D(SubmodulesModel);
    beginInsertRows(QModelIndex(), d->list.size(), d->list.size());
    d->list.append(module);
    endInsertRows();
    return true;
}

const Git::Submodule &SubmodulesModel::fromIndex(const QModelIndex &index)
{
    Q_D(SubmodulesModel);
    if (!index.isValid() || index.row() < 0 || index.row() >= d->list.size())
        return nullptr;

    return d->list.at(index.row());
}

void SubmodulesModel::reload()
{
    Q_D(SubmodulesModel);
    if (d->manager->isValid())
        d->list = d->manager->submodules()->allSubmodules();
    else
        d->list.clear();
}

SubmodulesModelPrivate::SubmodulesModelPrivate(SubmodulesModel *parent, Git::Repository *manager)
    : manager{manager}
    , cache{manager->submodules()}
    , q_ptr{parent}
{
}

#include "moc_submodulesmodel.cpp"
