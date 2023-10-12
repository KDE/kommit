/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submodule.h"

#include <git2/submodule.h>

namespace Git
{

Submodule::Submodule() = default;

Submodule::Submodule(git_submodule *submodule)
{
    mName = QString{git_submodule_name(submodule)};
    mPath = QString{git_submodule_path(submodule)};
    mUrl = QString{git_submodule_url(submodule)};

    auto headId = git_submodule_head_id(submodule);
    mRefName = QString{git_oid_tostr_s(headId)};
}

const QString &Submodule::path() const
{
    return mPath;
}

void Submodule::setPath(const QString &newPath)
{
    mPath = newPath;
}

const QString &Submodule::commitHash() const
{
    return mCommitHash;
}

void Submodule::setCommitHash(const QString &newCommitHash)
{
    mCommitHash = newCommitHash;
}

const QString &Submodule::refName() const
{
    return mRefName;
}

void Submodule::setRefName(const QString &newRefName)
{
    mRefName = newRefName;
}

const QString &Submodule::url() const
{
    return mUrl;
}

void Submodule::setUrl(const QString &newUrl)
{
    mUrl = newUrl;
}

QString Submodule::name() const
{
    return mName;
}

} // namespace Git
