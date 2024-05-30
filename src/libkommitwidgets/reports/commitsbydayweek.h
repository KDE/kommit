/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractreport.h"
#include "libkommitwidgets_export.h"

class LIBKOMMITWIDGETS_EXPORT CommitsByDayWeek : public AbstractReport
{
    Q_OBJECT

public:
    CommitsByDayWeek(Git::Manager *git, QObject *parent = nullptr);

    void reload() override;
    QString name() const override;

    int columnCount() const override;
    QStringList headerData() const override;

    bool supportChart() const override;
    QString axisXTitle() const override;
    QString axisYTitle() const override;

    int labelsAngle() const override;

private:
    enum CommitsByDayWeekRoles {
        DayOfWeek,
        Commits,
        LastColumn,
    };
};
