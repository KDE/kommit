/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsfiltermodel.h"

#include "entities/commit.h"
#include "models/logsmodel.h"

CommitsFilterModel::CommitsFilterModel(Git::LogsModel *sourceModel, QObject *parent)
    : QSortFilterProxyModel{parent}
    , mSourceModel{sourceModel}
{
    setSourceModel(sourceModel);
}

const QString &CommitsFilterModel::filterTerm() const
{
    return mFilterTerm;
}

void CommitsFilterModel::setFilterTerm(const QString &newFilterTerm)
{
    mFilterTerm = newFilterTerm;
    invalidateFilter();
}

bool CommitsFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_parent);

    if (mFilterTerm.isEmpty())
        return true;

    const auto &log = mSourceModel->at(source_row);

    return log->message().contains(mFilterTerm) || log->commitHash().contains(mFilterTerm) || log->body().contains(mFilterTerm)
        || log->author()->email().contains(mFilterTerm) || log->author()->name().contains(mFilterTerm);
}

#include "moc_commitsfiltermodel.cpp"
