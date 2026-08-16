/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractgititemsmodel.h"
#include "repository.h"

AbstractGitItemsModel::AbstractGitItemsModel(Git::Repository *git)
    : AbstractGitItemsModel{git, git}
{
}

AbstractGitItemsModel::AbstractGitItemsModel(Git::Repository *git, QObject *parent)
    : QAbstractListModel(parent)
    , mGit(git)
{
    connect(git, &Git::Repository::pathChanged, this, &AbstractGitItemsModel::repositoryChanged);
}

void AbstractGitItemsModel::repositoryChanged()
{
    mStale = true;

    if (!mLoadOnDemand) {
        load();
        return;
    }

    // Drop the previous repository's rows straight away, so nothing shows stale content
    // while the real load waits for someone to ask for it.
    clear();
    setStatus(NotLoaded);
}

bool AbstractGitItemsModel::loadOnDemand() const
{
    return mLoadOnDemand;
}

void AbstractGitItemsModel::setLoadOnDemand(bool loadOnDemand)
{
    mLoadOnDemand = loadOnDemand;
}

void AbstractGitItemsModel::loadIfNeeded()
{
    if (mStale || m_status != Loaded)
        load();
}

bool AbstractGitItemsModel::isLoaded() const
{
    return m_status == Loaded;
}

AbstractGitItemsModel::Status AbstractGitItemsModel::status() const
{
    return m_status;
}

void AbstractGitItemsModel::clear()
{
}

Git::Repository *AbstractGitItemsModel::manager() const
{
    return mGit;
}

void AbstractGitItemsModel::load()
{
    setStatus(Loading);
    beginResetModel();
    reload();
    endResetModel();
    mStale = false;
    setStatus(Loaded);
}

void AbstractGitItemsModel::setStatus(Status newStatus)
{
    if (m_status == newStatus)
        return;
    m_status = newStatus;

    if (m_status == Loaded)
        Q_EMIT loaded();

    Q_EMIT statusChanged();
}

#include "moc_abstractgititemsmodel.cpp"
