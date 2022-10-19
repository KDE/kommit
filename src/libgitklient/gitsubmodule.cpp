/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitsubmodule.h"

namespace Git
{

Submodule::Submodule() = default;

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

} // namespace Git
