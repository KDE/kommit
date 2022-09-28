/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submodulesmodel.h"
#include "git/gitmanager.h"
#include "git/gitsubmodule.h"
#include <KLocalizedString>

namespace Git
{

SubmodulesModel::SubmodulesModel(Git::Manager *git, QObject *parent)
    : AbstractGitItemsModel{git, parent}
{
}

int SubmodulesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int SubmodulesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant SubmodulesModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    auto submodule = mData.at(index.row());

    switch (index.column()) {
    case 0:
        return submodule->path();
    case 1:
        return submodule->refName();
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

bool SubmodulesModel::append(Submodule *module)
{
    beginInsertRows(QModelIndex(), mData.size(), mData.size());
    mData.append(module);
    endInsertRows();
    return true;
}

Submodule *SubmodulesModel::fromIndex(const QModelIndex &index)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return nullptr;

    return mData.at(index.row());
}

void SubmodulesModel::fill()
{
    qDeleteAll(mData);
    mData.clear();
    const auto modulesList = mGit->readAllNonEmptyOutput({QStringLiteral("submodule"), QStringLiteral("status")});
    for (const auto &line : modulesList) {
        auto m = new Submodule;
        m->setCommitHash(line.mid(0, 40));
        auto n = line.lastIndexOf(QLatin1Char(' '));
        if (line.count(QLatin1Char(' ')) == 1)
            n = line.size();
        m->setPath(line.mid(41, n - 41));

        if (line.count(QLatin1Char(' ')) == 2)
            m->setRefName(line.mid(n));
        mData.append(m);
    }
}

} // namespace Git
