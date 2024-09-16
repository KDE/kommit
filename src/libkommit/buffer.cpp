/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "buffer.h"

#include <git2/buffer.h>

namespace Git
{

Buf::~Buf()
{
    git_buf_dispose(&buf);
}

git_buf *Buf::operator&()
{
    return &buf;
}

QString Buf::toString() const
{
    return QString{buf.ptr};
}

}
