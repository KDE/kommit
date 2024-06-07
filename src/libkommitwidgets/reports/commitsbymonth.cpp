/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsbymonth.h"

#include <KLocalizedString>

#include <entities/commit.h>
#include <entities/signature.h>
#include <gitmanager.h>

CommitsByMonth::CommitsByMonth(Git::Manager *git, QObject *parent)
    : AbstractReport{git, parent}
{
    setColumnCount(2);
}

void CommitsByMonth::reload()
{
    clear();

    struct DataRow {
        int sortKey;
        int count{0};
        QString month;
    };

    QList<DataRow> data;

    auto commitCb = [&data](QSharedPointer<Git::Commit> commit) {
        auto time = commit->committer()->time();

        auto sortKey = time.toString(QStringLiteral("yyyyMM")).toInt();
        auto i = std::find_if(data.begin(), data.end(), [&sortKey](const DataRow &row) {
            return row.sortKey == sortKey;
        });
        if (i == data.end()) {
            DataRow row;
            row.sortKey = sortKey;
            row.month = time.toString(QStringLiteral("MMM-yy"));
            data << row;
        } else {
            (*i).count++;
        }
    };

    mGit->forEachCommits(commitCb, {});

    for (auto const &d : data) {
        addData({d.month, d.count});
        extendRange(d.count);
    }
}

QString CommitsByMonth::name() const
{
    return i18n("Commits by month");
}

int CommitsByMonth::columnCount() const
{
    return 2;
}

QStringList CommitsByMonth::headerData() const
{
    return {i18n("Month"), i18n("Commits")};
}

bool CommitsByMonth::supportChart() const
{
    return true;
}

QString CommitsByMonth::axisXTitle() const
{
    return i18n("Month-year");
}

QString CommitsByMonth::axisYTitle() const
{
    return i18n("Commits");
}

int CommitsByMonth::labelsAngle() const
{
    return 90;
}

#include "moc_commitsbymonth.cpp"
