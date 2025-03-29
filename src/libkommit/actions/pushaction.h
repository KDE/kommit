/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractaction.h"

#include "libkommit_export.h"

#include <QSharedPointer>

namespace Git {

class PushActionPrivate;
class LIBKOMMIT_EXPORT PushAction : public AbstractAction
{
public:
    PushAction();

protected:
    int exec();

private:
    QSharedPointer<PushActionPrivate> d;
};
}
