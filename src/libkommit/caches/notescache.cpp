/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "notescache.h"
#include "entities/commit.h"

namespace Git
{

NotesCache::NotesCache(git_repository *repo)
    : Git::AbstractCache<Note>{repo}
{
}

};
