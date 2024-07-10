/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashesmodel.h"

#include "caches/stashescache.h"
#include "entities/commit.h"
#include "entities/signature.h"
#include "gitmanager.h"
#include <KLocalizedString>

#include <git2/stash.h>

StashesModel::StashesModel(Git::Manager *git, QObject *parent)
    : AbstractGitItemsModel(git, parent)
{
}

int StashesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int StashesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(StashesModel::LastColumn) + 1;
}

QVariant StashesModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    auto remote = mData.at(index.row());

    switch (index.column()) {
    case Subject:
        return remote->message();
    case CommitHash:
        return remote->commit()->commitHash();
    case AuthorName:
        return remote->commit()->author()->name();
    case AuthorEmail:
        return remote->commit()->author()->email();
    case Time:
        return remote->commit()->author()->time();
    }
    return {};
}

QVariant StashesModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section) {
        case Subject:
            return i18n("Subject");
        case CommitHash:
            return i18n("Commit");
        case AuthorName:
            return i18n("Author name");
        case AuthorEmail:
            return i18n("Author email");
        case Time:
            return i18n("Time");
        }

    return {};
}

QSharedPointer<Git::Stash> StashesModel::fromIndex(const QModelIndex &index) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    return mData.at(index.row());
}

void StashesModel::clear()
{
    beginResetModel();
    mData.clear();
    endResetModel();
}

void StashesModel::reload()
{
    mData = mGit->stashes()->allStashes();
}

#include "moc_stashesmodel.cpp"
