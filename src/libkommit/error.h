/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Error
{
public:
    static int klass();
    static QString message();
};

}
