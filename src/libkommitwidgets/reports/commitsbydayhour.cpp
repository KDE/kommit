/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsbydayhour.h"

#include <KLocalizedString>

#include <entities/commit.h>
#include <entities/signature.h>
#include <gitmanager.h>

CommitsByDayHour::CommitsByDayHour(Git::Manager *git, QObject *parent)
    : AbstractReport{git, parent}
{
}

void CommitsByDayHour::reload()
{
    beginResetModel();
    mData.clear();

    for (int i = 0; i < 24; ++i)
        mData.insert(i, 0);

    auto commitCb = [this](QSharedPointer<Git::Commit> commit) {
        auto time = commit->committer()->time();

        mData[time.time().hour()] = mData[time.time().hour()] + 1;
    };

    mGit->forEachCommits(commitCb, "");
    endResetModel();
}

QString CommitsByDayHour::name() const
{
    return i18n("Commits by hour of day");
}

int CommitsByDayHour::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 24;
}

int CommitsByDayHour::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant CommitsByDayHour::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case Hour:
        return i18n("Hour");
    case Commits:
        return i18n("Commits");
    }
    return {};
}

QVariant CommitsByDayHour::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    switch (index.column()) {
    case Hour: {
        return index.row();
    }
    case Commits:
        return mData[index.row()];
    }

    return {};
}
