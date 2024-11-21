/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "hunk.h"

namespace Diff
{

DiffHunk::DiffHunk()
    : type{}
    , right{}
    , left{}
{
}

int DiffHunk::count() const
{
    return 2;
}

Range DiffHunk::get(int index) const
{
    switch (index) {
    case 0:
        return left;
    case 1:
        return right;
    }
    return {};
}

MergeHunk::MergeHunk()
    : base{}
    , local{}
    , remote{}
{
}

int MergeHunk::count() const
{
    return 3;
}

Range MergeHunk::get(int index) const
{
    switch (index) {
    case 0:
        return base;
    case 1:
        return local;
    case 2:
        return remote;
    }
    return {};
}

Hunk::Hunk()
{
}

Hunk::~Hunk()
{
}
}
