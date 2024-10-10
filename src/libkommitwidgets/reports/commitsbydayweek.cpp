/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsbydayweek.h"

#include <KLocalizedString>
#include <kommitwidgetsglobaloptions.h>

#include <entities/commit.h>
#include <entities/signature.h>
#include <repository.h>

namespace
{
QString dayToString(Qt::DayOfWeek dayOfWeek)
{
    switch (dayOfWeek) {
    case Qt::Monday:
        return i18n("Monday");
    case Qt::Tuesday:
        return i18n("Tuesday");
    case Qt::Wednesday:
        return i18n("Wednesday");
    case Qt::Thursday:
        return i18n("Thursday");
    case Qt::Friday:
        return i18n("Friday");
    case Qt::Saturday:
        return i18n("Saturday");
    case Qt::Sunday:
        return i18n("Sunday");
    }
    return QString();
}
}

CommitsByDayWeek::CommitsByDayWeek(Git::Repository *git, QObject *parent)
    : AbstractReport{git, parent}
{
}

void CommitsByDayWeek::reload()
{
    clear();

    if (mGit->isValid()) {
        QMap<Qt::DayOfWeek, int> map;

        map.insert(Qt::Monday, 0);
        map.insert(Qt::Tuesday, 0);
        map.insert(Qt::Wednesday, 0);
        map.insert(Qt::Thursday, 0);
        map.insert(Qt::Friday, 0);
        map.insert(Qt::Saturday, 0);
        map.insert(Qt::Sunday, 0);
        auto commitCb = [&map](QSharedPointer<Git::Commit> commit) {
            auto time = commit->committer()->time();

            auto count = map.value(static_cast<Qt::DayOfWeek>(time.date().dayOfWeek()), 0);
            map[static_cast<Qt::DayOfWeek>(time.date().dayOfWeek())] = count + 1;
        };

        mGit->forEachCommits(commitCb, {});

        for (auto d = map.constBegin(); d != map.constEnd(); ++d) {
            addData({dayToString(d.key()), d.value()});
            extendRange(d.value());
        }
    }

    Q_EMIT reloaded();
}

QString CommitsByDayWeek::name() const
{
    return i18n("Commits by day of week");
}

int CommitsByDayWeek::columnCount() const
{
    return 2;
}

QStringList CommitsByDayWeek::headerData() const
{
    return {i18n("Day"), i18n("Commits")};
}

bool CommitsByDayWeek::supportChart() const
{
    return true;
}

QString CommitsByDayWeek::axisXTitle() const
{
    return i18n("Day of week");
}

QString CommitsByDayWeek::axisYTitle() const
{
    return i18n("Commits");
}

int CommitsByDayWeek::labelsAngle() const
{
    return 90;
}

#include "moc_commitsbydayweek.cpp"
