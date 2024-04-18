/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsbydayweek.h"

#include <KLocalizedString>
#include <kommitwidgetsglobaloptions.h>

#include <entities/commit.h>
#include <entities/signature.h>
#include <gitmanager.h>

CommitsByDayWeek::CommitsByDayWeek(Git::Manager *git, QObject *parent)
    : AbstractReport{git, parent}
{
}

void CommitsByDayWeek::reload()
{
    beginResetModel();
    mData.clear();

    auto commitCb = [this](QSharedPointer<Git::Commit> commit) {
        auto time = commit->committer()->time();

        auto count = mData.value(static_cast<Qt::DayOfWeek>(time.date().dayOfWeek()), 0);
        mData[static_cast<Qt::DayOfWeek>(time.date().dayOfWeek())] = count + 1;
    };

    mGit->forEachCommits(commitCb, "");
    endResetModel();
}

QString CommitsByDayWeek::name() const
{
    return i18n("Commits by day of week");
}

int CommitsByDayWeek::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 7;
}

int CommitsByDayWeek::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant CommitsByDayWeek::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return {};

    switch (section) {
    case DayOfWeek:
        return i18n("Day");
    case Commits:
        return i18n("Commits");
    }
    return {};
}

QVariant CommitsByDayWeek::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole || !index.isValid() || index.row() < 0 || index.row() >= mData.size())
        return {};

    switch (index.column()) {
    case DayOfWeek: {
        auto dayOfWeek = static_cast<Qt::DayOfWeek>(index.row() + 1);

        // TODO: convert to QCalendar::weekDayName
        switch (dayOfWeek) {
        case Qt::Monday:
            return "Monday";
        case Qt::Tuesday:
            return "Tuesday";
        case Qt::Wednesday:
            return "Wednesday";
        case Qt::Thursday:
            return "Thursday";
        case Qt::Friday:
            return "Friday";
        case Qt::Saturday:
            return "Saturday";
        case Qt::Sunday:
            return "Sunday";
        }
        break;
    }
    case Commits:
        return mData[static_cast<Qt::DayOfWeek>(index.row())];
    }

    return {};
}
