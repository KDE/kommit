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
    explicit SubmodulePrivate();
    explicit SubmodulePrivate(git_submodule *submodule, git_repository *repo = nullptr);
    explicit SubmodulePrivate(const char *name, git_repository *repo = nullptr);
    ~SubmodulePrivate();

    git_submodule *submodule{nullptr};
    git_repository *repo{nullptr};

    bool own{false};
    QString name;
    QString path;
    QString url;
    Oid headId;
    QString branch;
    Submodule::Status status;

    void fillData();
};

SubmodulePrivate::SubmodulePrivate()
    : own{false}
{
}

SubmodulePrivate::SubmodulePrivate(git_submodule *submodule, git_repository *repo)
    : submodule{submodule}
    , repo{repo}
    , own{true}
{
    fillData();
}

SubmodulePrivate::SubmodulePrivate(const char *name, git_repository *repo)
    : repo{repo}
    , own{true}
{
    git_submodule_lookup(&submodule, repo, name);
    fillData();
}

SubmodulePrivate::~SubmodulePrivate()
{
    if (own)
        git_submodule_free(submodule);
}

void SubmodulePrivate::fillData()
{
    if (submodule) {
#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) >= QT_VERSION_CHECK(1, 2, 0)
        git_submodule *sm;
        git_submodule_dup(&sm, submodule);
        submodule = sm;
        own = true;
#endif

        name = QString{git_submodule_name(submodule)};
        path = QString{git_submodule_path(submodule)};
        url = QString{git_submodule_url(submodule)};
        branch = QString{git_submodule_branch(submodule)};
        headId = Oid{git_submodule_head_id(submodule)};

        if (!repo && submodule)
            this->repo = git_submodule_owner(submodule);

        unsigned int st;
        git_submodule_status(&st, repo, git_submodule_name(submodule), GIT_SUBMODULE_IGNORE_UNSPECIFIED);
        status = static_cast<Submodule::Status>(st);

#if QT_VERSION_CHECK(LIBGIT2_VER_MAJOR, LIBGIT2_VER_MINOR, LIBGIT2_VER_MINOR) < QT_VERSION_CHECK(1, 2, 0)
        submodule = nullptr;
#endif
    }
}

Submodule::Submodule()
    : d{new SubmodulePrivate}
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
    if (d->submodule)
        return QString{git_submodule_path(d->submodule)};
    return d->path;
}

Oid Submodule::headId() const
{
    if (d->submodule)
        return Oid{git_submodule_head_id(d->submodule)};
    return d->headId;
}

QString Submodule::url() const
{
    if (d->submodule)
        return QString{git_submodule_url(d->submodule)};
    return d->url;
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
    if (d->submodule)
        return QString{git_submodule_name(d->submodule)};
    return d->name;
}

QString Submodule::branch()
{
    if (d->submodule)
        return QString{git_submodule_branch(d->submodule)};
    return d->branch;
}

Submodule::StatusFlags Submodule::status() const
{
    if (d->submodule) {
        unsigned int st;
        git_submodule_status(&st, d->repo, git_submodule_name(d->submodule), GIT_SUBMODULE_IGNORE_UNSPECIFIED);
        return static_cast<Submodule::Status>(st);
    }
    return d->status;
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
