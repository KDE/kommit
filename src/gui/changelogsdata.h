/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QStringList>
#include <QVersionNumber>
#include <QScopedPointer>

class ChangeLogsDataPrivate;
class ChangeLogsData
{
public:
    ChangeLogsData();
    virtual ~ChangeLogsData();

    [[nodiscard]] QString generateMarkdown(const QVersionNumber &until = {}) const;

private:
    QScopedPointer<ChangeLogsDataPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ChangeLogsData)
};
