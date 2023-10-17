/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>

#include "checkoutoptions.h"
#include "fetchoptions.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT AddSubmoduleOptions : public FetchOptions, public CheckoutOptions
{
public:
    AddSubmoduleOptions();

    QString url;
    QString path;
};

}
