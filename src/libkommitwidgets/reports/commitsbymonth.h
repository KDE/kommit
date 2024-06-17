/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractreport.h"
#include "libkommitwidgets_export.h"

class LIBKOMMITWIDGETS_EXPORT CommitsByMonth : public AbstractReport
{
    Q_OBJECT
public:
    CommitsByMonth(Git::Manager *git, QObject *parent = nullptr);

    void reload() override;
    Q_REQUIRED_RESULT QString name() const override;

    Q_REQUIRED_RESULT int columnCount() const override;
    Q_REQUIRED_RESULT QStringList headerData() const override;

    Q_REQUIRED_RESULT bool supportChart() const override;

    Q_REQUIRED_RESULT QString axisXTitle() const override;
    Q_REQUIRED_RESULT QString axisYTitle() const override;

    Q_REQUIRED_RESULT int labelsAngle() const override;
};
