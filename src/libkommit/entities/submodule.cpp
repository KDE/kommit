/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/submodule.h"
#include "gitglobal_p.h"
#include "qdebug.h"
#include "types.h"

#include <git2/submodule.h>

#include <gitmanager.h>

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

Submodule::Submodule(git_repository *repo, git_submodule *submodule)
    : ptr{submodule}
    , mRepo{repo}
{
    mName = QString{git_submodule_name(submodule)};
    mPath = QString{git_submodule_path(submodule)};
    mUrl = QString{git_submodule_url(submodule)};

    auto headId = git_submodule_head_id(submodule);
    mRefName = QString{git_oid_tostr_s(headId)};
}

Submodule::~Submodule()
{
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
    auto repo = mRepo;
    if (!repo)
        repo = git_submodule_owner(ptr);
    if (git_submodule_status(&status, repo, mName.toLocal8Bit().constData(), GIT_SUBMODULE_IGNORE_UNSPECIFIED)) {
        auto err = git_error_last();
        qDebug() << err->klass << QString{err->message};
        return Status::Unknown;
    }

    return static_cast<StatusFlags>(status);
}

QStringList Submodule::statusTexts() const
{
    auto status = this->status();
    QStringList list;

    // TODO i18n ?
    if (status & Status::InHead)
        list << QStringLiteral("superproject head contains submodule");
    if (status & Status::InIndex)
        list << QStringLiteral("superproject index contains submodule");
    if (status & Status::InConfig)
        list << QStringLiteral("superproject gitmodules has submodule");
    if (status & Status::InWd)
        list << QStringLiteral("superproject workdir has submodule");
    if (status & Status::IndexAdded)
        list << QStringLiteral("in index, not in head");
    if (status & Status::IndexDeleted)
        list << QStringLiteral("in head, not in index");
    if (status & Status::IndexModified)
        list << QStringLiteral("index and head don't match");
    if (status & Status::WdUninitialized)
        list << QStringLiteral("workdir contains empty directory");
    if (status & Status::WdAdded)
        list << QStringLiteral("in workdir, not index");
    if (status & Status::WdDeleted)
        list << QStringLiteral("in index, not workdir");
    if (status & Status::WdModified)
        list << QStringLiteral("index and workdir head don't match");
    if (status & Status::WdIndexModified)
        list << QStringLiteral("submodule workdir index is dirty");
    if (status & Status::WdWdModified)
        list << QStringLiteral("submodule workdir has modified files");
    if (status & Status::WdUntracked)
        list << QStringLiteral("wd contains untracked files");
    return list;
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

Manager *Submodule::open() const
{
    git_repository *repo;
    if (git_submodule_open(&repo, ptr))
        return nullptr;

    return new Git::Manager{repo};
}

} // namespace Git

#include "moc_submodule.cpp"
