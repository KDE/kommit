/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "entities/submodule.h"
#include "entities/oid.h"
#include "gitglobal_p.h"
#include "observers/fetchobserver.h"
#include "options/fetchoptions.h"
#include "qdebug.h"
#include "types.h"

#include <git2/remote.h>

#include <repository.h>

namespace Git
{

class SubmodulePrivate
{
public:
    explicit SubmodulePrivate(git_submodule *module, git_repository *repo = nullptr);
    ~SubmodulePrivate();

    git_submodule *submodule{nullptr};
    git_repository *repo{nullptr};
};

SubmodulePrivate::SubmodulePrivate(git_submodule *module, git_repository *repo)
    : submodule{module}
    , repo{repo}
{
    if (!repo && module)
        this->repo = git_submodule_owner(submodule);
}

SubmodulePrivate::~SubmodulePrivate()
{
    git_submodule_free(submodule);
}

Submodule::Submodule()
    : d{new SubmodulePrivate{nullptr, nullptr}}
{
}

Submodule::Submodule(git_submodule *submodule, git_repository *repo)
    : d{new SubmodulePrivate{submodule, repo}}
{
}

Submodule::Submodule(const Submodule &other)
    : d{other.d}
{
}

Submodule &Submodule::operator=(const Submodule &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Submodule::operator==(const Submodule &other) const
{
    return d->submodule == other.d->submodule;
}

bool Submodule::operator!=(const Submodule &other) const
{
    return !(*this == other);
}

bool Submodule::isNull() const
{
    return !d->submodule;
}

git_submodule *Submodule::data() const
{
    return d->submodule;
}

const git_submodule *Submodule::constData() const
{
    return d->submodule;
}

QString Submodule::path() const
{
    if (!d->submodule)
        return {};
    return QString{git_submodule_path(d->submodule)};
}

Oid Submodule::headId() const
{
    if (!d->submodule)
        return {};
    return Oid{git_submodule_head_id(d->submodule)};
}

QString Submodule::url() const
{
    if (!d->submodule)
        return {};
    return QString{git_submodule_url(d->submodule)};
}

void Submodule::setUrl(const QString &newUrl)
{
    if (!d->submodule)
        return;
    BEGIN;
    STEP git_submodule_set_url(d->repo, git_submodule_name(d->submodule), toConstChars(newUrl));
}

QString Submodule::name() const
{
    if (!d->submodule)
        return {};

    return QString{git_submodule_name(d->submodule)};
}

QString Submodule::branch()
{
    if (!d->submodule)
        return {};
    return QString{git_submodule_branch(d->submodule)};
}

Submodule::StatusFlags Submodule::status() const
{
    if (!d->submodule)
        return Status::Unknown;

    unsigned int status;

    if (git_submodule_status(&status, d->repo, git_submodule_name(d->submodule), GIT_SUBMODULE_IGNORE_UNSPECIFIED)) {
        auto err = git_error_last();
        qDebug() << err->klass << QString{err->message};
        return Status::Unknown;
    }

    return static_cast<StatusFlags>(status);
}

bool Submodule::hasModifiedFiles() const
{
    if (!d->submodule)
        return false;

    unsigned int status;

    BEGIN
    STEP git_submodule_status(&status, d->repo, git_submodule_name(d->submodule), GIT_SUBMODULE_IGNORE_UNSPECIFIED);
    END;

    RETURN_COND(GIT_SUBMODULE_STATUS_IS_WD_DIRTY(status), false);
}

Oid Submodule::indexId() const
{
    if (!d->submodule)
        return {};
    return Oid{git_submodule_index_id(d->submodule)};
}

Oid Submodule::workingDirectoryId() const
{
    if (!d->submodule)
        return {};
    return Oid{git_submodule_wd_id(d->submodule)};
}

bool Submodule::sync() const
{
    if (!d->submodule)
        return {};

    return !git_submodule_sync(d->submodule);
}

bool Submodule::reload(bool force) const
{
    if (!d->submodule)
        return false;
    return !git_submodule_reload(d->submodule, force);
}

Repository *Submodule::open() const
{
    if (!d->submodule)
        return nullptr;

    git_repository *repo;
    if (git_submodule_open(&repo, d->submodule))
        return nullptr;

    return new Git::Repository{repo};
}

bool Submodule::update(const FetchOptions &opts, FetchObserver *observer)
{
    if (!d->submodule)
        return {};

    git_submodule_update_options update_options = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

    opts.applyToFetchOptions(&update_options.fetch_opts);
    if (observer)
        observer->applyOfFetchOptions(&update_options.fetch_opts);

    auto r = git_submodule_update(d->submodule, 1, &update_options);

    if (observer)
        Q_EMIT observer->finished();

    return r == 0;
}

} // namespace Git

#include "moc_submodule.cpp"
