/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsbydayhour.h"

#include <KLocalizedString>

#include <entities/commit.h>
#include <entities/signature.h>
#include <repository.h>

CommitsByDayHour::CommitsByDayHour(Git::Repository *git, QObject *parent)
    : AbstractReport{git, parent}
{
    setColumnCount(2);
}

void CommitsByDayHour::reload()
{
    clear();

    if (mGit->isValid()) {
        QMap<int, int> map;
        for (int i = 0; i < 24; ++i)
            map.insert(i, 0);

        auto commitCb = [&map](QSharedPointer<Git::Commit> commit) {
            auto time = commit->committer()->time();

            auto c = map[time.time().hour()] + 1;
            map[time.time().hour()] = c;
        };

        mGit->forEachCommits(commitCb, {});

        for (auto d = map.constBegin(); d != map.constEnd(); ++d) {
            addData({d.key(), d.value()});
            extendRange(d.value());
        }
    }
    Q_EMIT reloaded();
}

QString CommitsByDayHour::name() const
{
    return i18n("Commits by hour of day");
}

int CommitsByDayHour::columnCount() const
{
    return 2;
}

QStringList CommitsByDayHour::headerData() const
{
    return {i18n("Hour"), i18n("Commits")};
}

bool CommitsByDayHour::supportChart() const
{
    return true;
}

QString CommitsByDayHour::axisXTitle() const
{
    return i18n("Hour of day");
}

QString CommitsByDayHour::axisYTitle() const
{
    return i18n("Commits");
}

#include "moc_commitsbydayhour.cpp"
