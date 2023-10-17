/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submodule.h"
#include "qdebug.h"
#include "types.h"

#include <git2/submodule.h>

namespace Git
{

Submodule::Submodule() = default;

Submodule::Submodule(git_submodule *submodule)
    : ptr{submodule}
{
    mName = QString{git_submodule_name(submodule)};
    mPath = QString{git_submodule_path(submodule)};
    mUrl = QString{git_submodule_url(submodule)};

    auto headId = git_submodule_head_id(submodule);
    mRefName = QString{git_oid_tostr_s(headId)};
}

Submodule::~Submodule()
{
    qDebug() << Q_FUNC_INFO << ptr;
    // git_submodule_free(ptr);
    ptr = nullptr;
}

const QString &Submodule::path() const
{
    return mPath;
}

const QString &Submodule::commitHash() const
{
    return mCommitHash;
}

const QString &Submodule::refName() const
{
    return mRefName;
}

QString Submodule::url() const
{
    auto s = QString{git_submodule_url(ptr)};
    return s;
}

void Submodule::setUrl(const QString &newUrl)
{
    mUrl = newUrl;
    BEGIN;
    STEP git_submodule_set_url(git_submodule_owner(ptr), toConstChars(mName), toConstChars(newUrl));
}

QString Submodule::name() const
{
    return mName;
}

QString Submodule::branch()
{
    if (mBranch.isNull())
        mBranch = git_submodule_branch(ptr);
    return mBranch;
}

Submodule::StatusFlags Submodule::status() const
{
    unsigned int status;
    if (git_submodule_status(&status, git_submodule_owner(ptr), mName.toLocal8Bit().constData(), GIT_SUBMODULE_IGNORE_UNSPECIFIED))
        return Status::Unknown;

    return static_cast<StatusFlags>(status);
}

bool Submodule::sync() const
{
    if (Q_UNLIKELY(!ptr))
        return false;
    return !git_submodule_sync(ptr);
}

bool Submodule::reload(bool force) const
{
    if (Q_UNLIKELY(!ptr))
        return false;
    return !git_submodule_reload(ptr, force);
}

} // namespace Git
