/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/types.h>

#include <QString>

namespace Git
{

class Buf
{
    git_buf buf{0};

public:
    ~Buf();
    git_buf *operator&();
    QString toString() const;
};
}
