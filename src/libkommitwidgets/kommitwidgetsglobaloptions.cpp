/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "kommitwidgetsglobaloptions.h"

KommitWidgetsGlobalOptions::KommitWidgetsGlobalOptions()
{
}

void KommitWidgetsGlobalOptions::setColor(Git::ChangeStatus status, const QColor &color)
{
    mColors.insert(status, color);
}

QColor KommitWidgetsGlobalOptions::statucColor(Git::ChangeStatus status) const
{
    return mColors.value(status);
}

QCalendar KommitWidgetsGlobalOptions::calendar() const
{
    return mCalendar;
}

void KommitWidgetsGlobalOptions::setCalendar(const QCalendar &calendar)
{
    mCalendar = calendar;
}

KommitWidgetsGlobalOptions *KommitWidgetsGlobalOptions::instance()
{
    static KommitWidgetsGlobalOptions *instance = nullptr;
    if (!instance)
        instance = new KommitWidgetsGlobalOptions;

    return instance;
}
