/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QCalendar>
#include <QColor>
#include <QMap>

#include "libkommit_global.h"
#include "libkommitwidgets_export.h"
#include "types.h"

class LIBKOMMITWIDGETS_EXPORT KommitWidgetsGlobalOptions
{
public:
    KommitWidgetsGlobalOptions();

    void setColor(Git::ChangeStatus status, const QColor &color);
    [[nodiscard]] QColor statucColor(Git::ChangeStatus status) const;

    [[nodiscard]] QCalendar calendar() const;
    void setCalendar(const QCalendar &calendar);

    static KommitWidgetsGlobalOptions *instance();

private:
    QMap<Git::ChangeStatus, QColor> mColors;
    QCalendar mCalendar;
};
