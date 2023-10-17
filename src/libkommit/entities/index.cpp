/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "index.h"

#include "types.h"

#include <git2/index.h>

namespace Git
{

Index::Index(git_index *index)
    : ptr{index}
{
}

bool Index::addByPath(const QString &path) const
{
    return !git_index_add_bypath(ptr, toConstChars(path));
}

bool Index::removeByPath(const QString &path) const
{
    return !git_index_remove_bypath(ptr, toConstChars(path));
}

}
