/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractreport.h"
#include "libkommit_export.h"

class LIBKOMMIT_EXPORT CommitsByDayHour : public AbstractReport
{
    Q_OBJECT
public:
    CommitsByDayHour(Git::Manager *git, QObject *parent = nullptr);

    void reload() override;
    QString name() const override;

    int columnCount() const override;
    QStringList headerData() const override;

    bool supportChart() const override;

private:
    enum CommitsByDayHourRoles {
        Hour,
        Commits,
        LastColumn,
    };
};
