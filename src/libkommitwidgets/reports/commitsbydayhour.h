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

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;

private:
    enum CommitsByDayHourRoles {
        Hour,
        Commits,
        LastColumn,
    };

    QMap<int, int> mData;
};
