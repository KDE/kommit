/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submodulesmodel.h"
#include "caches/submodulescache.h"
#include "entities/submodule.h"
#include "gitmanager.h"
#include <KLocalizedString>

class SubmodulesModelPrivate
{
public:
    explicit SubmodulesModelPrivate(SubmodulesModel *parent, Git::Manager *manager);
    QList<QSharedPointer<Git::Submodule>> list;
    Git::Manager *manager;
    Git::SubmodulesCache *cache;

    SubmodulesModel *q_ptr;
    Q_DECLARE_PUBLIC(SubmodulesModel)
};

SubmodulesModel::SubmodulesModel(Git::Manager *manager)
    : AbstractGitItemsModel{manager}
    , d_ptr{new SubmodulesModelPrivate{this, manager}}
{
    connect(manager->submodules(), &Git::SubmodulesCache::added, this, &SubmodulesModel::append);
}

SubmodulesModel::~SubmodulesModel()
{
    Q_D(SubmodulesModel);
    delete d;
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
        return submodule->statusTexts().join(QLatin1Char('\n'));
    }
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0:
            return submodule->path();
        case 1:
            return submodule->branch();
        case 2:
            return submodule->statusTexts().join(QStringLiteral(", "));
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

bool SubmodulesModel::append(QSharedPointer<Git::Submodule> module)
{
    Q_D(SubmodulesModel);
    beginInsertRows(QModelIndex(), d->list.size(), d->list.size());
    d->list.append(module);
    endInsertRows();
    return true;
}

QSharedPointer<Git::Submodule> SubmodulesModel::fromIndex(const QModelIndex &index)
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

SubmodulesModelPrivate::SubmodulesModelPrivate(SubmodulesModel *parent, Git::Manager *manager)
    : manager{manager}
    , cache{manager->submodules()}
    , q_ptr{parent}
{
}

#include "moc_submodulesmodel.cpp"
