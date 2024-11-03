/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <Kommit/Oid>

namespace Git
{

class LIBKOMMIT_EXPORT IOid
{
public:
    virtual ~IOid();
    virtual Oid oid() const = 0;
};

inline IOid::~IOid()
{
}

}
