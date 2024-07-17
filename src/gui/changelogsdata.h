/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"

#include <QStringList>
#include <QVersionNumber>

class ChangeLogsDataPrivate;
class LIBKOMMITWIDGETS_EXPORT ChangeLogsData
{
public:
    ChangeLogsData();
    ~ChangeLogsData();

    QString generateMarkdown(const QVersionNumber &until = {}) const;

private:
    ChangeLogsDataPrivate *d_ptr;
    Q_DECLARE_PRIVATE(ChangeLogsData)
};
