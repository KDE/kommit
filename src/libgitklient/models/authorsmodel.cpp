/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "authorsmodel.h"
#include <KLocalizedString>

namespace Git
{

AuthorsModel::AuthorsModel(Manager *git, QObject *parent)
    : AbstractGitItemsModel{git, parent}
{
}

int AuthorsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int AuthorsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(AuthorsModelsRoles::LastColumn) + 1;
}

QVariant AuthorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case Name:
        return i18n("Name");
    case Email:
        return i18n("Email");
    case Commits:
        return i18n("Commits");
    }
    return {};
}

QVariant AuthorsModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    switch (index.column()) {
    case Name:
        return mData.at(index.row())->name;
    case Email:
        return mData.at(index.row())->email;
    case Commits:
        return mData.at(index.row())->commits;
    }

    return {};
}

Author *AuthorsModel::findOrCreate(const QString &name, const QString &email)
{
    QMutexLocker locker(&mDataMutex);

    for (const auto &a : std::as_const(mData))
        if (a->email == email && a->name == name)
            return a;
    auto author = new Author;
    author->name = name;
    author->email = email;
    beginInsertRows(QModelIndex(), mData.size(), mData.size());
    mData.append(author);
    endInsertRows();
    return author;
}

void AuthorsModel::fill()
{
}

} // namespace Git
