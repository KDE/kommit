/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blamedata.h"

namespace Git
{

BlameData::BlameData() = default;

bool operator==(const BlameDataRow &l, const BlameDataRow &r)
{
    return l.log == r.log;
}
bool operator!=(const BlameDataRow &l, const BlameDataRow &r)
{
    return !(l.log == r.log);
}
}
