/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagscache.h"
#include "entities/tag.h"
#include "gitglobal_p.h"

#include <git2/revparse.h>
#include <git2/tag.h>

namespace Git
{

TagsCache::TagsCache(git_repository *repo)
    : Git::AbstractCache<Tag>{repo}
{
}

Tag *TagsCache::lookup(const QString &key, git_repository *repo)
{
    git_tag *tag;
    git_object *tagObject;

    BEGIN
    STEP git_revparse_single(&tagObject, repo, key.toLatin1().constData());
    STEP git_tag_lookup(&tag, repo, git_object_id(tagObject));

    if (IS_OK)
        return new Tag{tag};
    return nullptr;
}

};
