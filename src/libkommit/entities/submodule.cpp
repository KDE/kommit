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

struct GitSubmodulePointerDeleter {
    static inline void cleanup(git_submodule *sm)
    {
        git_submodule_free(sm);
    }
};

class SubmodulePrivate
{
    Submodule *q_ptr;
    Q_DECLARE_PUBLIC(Submodule)
public:
    explicit SubmodulePrivate(Submodule *parent, git_repository *repo = nullptr);
    ~SubmodulePrivate();

    git_repository *repo{nullptr};
    git_submodule *submodule{nullptr};
    QString name;
    QString url;
    QString path;
    QString refName;
    QString branch;
    QScopedPointer<git_submodule, GitSubmodulePointerDeleter> find() const; // TODO: do we need this?
};

Submodule::Submodule()
    : d{new SubmodulePrivate{this, nullptr}}
{
}

Submodule::Submodule(git_submodule *submodule, git_repository *repo)
    : d{new SubmodulePrivate{this, repo}}
{
    d->submodule = submodule;
    d->name = QString{git_submodule_name(submodule)};
    d->path = QString{git_submodule_path(submodule)};
    d->url = QString{git_submodule_url(submodule)};

    auto headId = git_submodule_head_id(submodule);
    d->refName = QString{git_oid_tostr_s(headId)};

    if (!d->repo)
        d->repo = git_submodule_owner(submodule);
    d->branch = git_submodule_branch(submodule);
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

const QString &Submodule::path() const
{
    return d->path;
}

const QString &Submodule::refName() const
{
    return d->refName;
}

QString Submodule::url() const
{
    return d->url;
}

void Submodule::setUrl(const QString &newUrl)
{
    d->url = newUrl;
    BEGIN;
    STEP git_submodule_set_url(d->repo, toConstChars(d->name), toConstChars(newUrl));
}

QString Submodule::name() const
{
    return d->name;
}

QString Submodule::branch()
{
    return d->branch;
}

Submodule::StatusFlags Submodule::status() const
{
    unsigned int status;

    if (git_submodule_status(&status, d->repo, d->name.toLocal8Bit().constData(), GIT_SUBMODULE_IGNORE_UNSPECIFIED)) {
        auto err = git_error_last();
        qDebug() << err->klass << QString{err->message};
        return Status::Unknown;
    }

    return static_cast<StatusFlags>(status);
}

bool Submodule::hasModifiedFiles() const
{
    unsigned int status;

    BEGIN
    STEP git_submodule_status(&status, d->repo, d->name.toLocal8Bit().constData(), GIT_SUBMODULE_IGNORE_UNSPECIFIED);
    END;

    RETURN_COND(GIT_SUBMODULE_STATUS_IS_WD_DIRTY(status), false);
}

Oid Submodule::headId()
{
    auto submodule = d->find();
    return Oid{git_submodule_head_id(submodule.get())};
}

Oid Submodule::indexId()
{
    auto submodule = d->find();
    return Oid{git_submodule_index_id(submodule.get())};
}

Oid Submodule::workingDirectoryId()
{
    auto submodule = d->find();
    return Oid{git_submodule_wd_id(submodule.get())};
}

bool Submodule::sync() const
{
    auto submodule = d->find();

    if (Q_UNLIKELY(!submodule))
        return false;

    return !git_submodule_sync(submodule.get());
}

bool Submodule::reload(bool force) const
{
    auto submodule = d->find();
    if (Q_UNLIKELY(!submodule))
        return false;
    return !git_submodule_reload(submodule.get(), force);
}

Repository *Submodule::open() const
{
    auto submodule = d->find();
    git_repository *repo;
    if (git_submodule_open(&repo, submodule.get()))
        return nullptr;

    return new Git::Repository{repo};
}

bool Submodule::update(const FetchOptions &opts, FetchObserver *observer)
{
    git_submodule_update_options update_options = GIT_SUBMODULE_UPDATE_OPTIONS_INIT;

    opts.applyToFetchOptions(&update_options.fetch_opts);
    if (observer)
        observer->applyOfFetchOptions(&update_options.fetch_opts);

    auto submodule = d->find();
    auto r = git_submodule_update(submodule.get(), 1, &update_options);

    if (observer)
        Q_EMIT observer->finished();

    return r == 0;
}

SubmodulePrivate::SubmodulePrivate(Submodule *parent, git_repository *repo)
    : q_ptr{parent}
    , repo{repo}
{
}

SubmodulePrivate::~SubmodulePrivate()
{
    git_submodule_free(submodule);
}

QScopedPointer<git_submodule, GitSubmodulePointerDeleter> SubmodulePrivate::find() const
{
    git_submodule *sm;

    git_submodule_lookup(&sm, repo, name.toUtf8().data());

    return QScopedPointer<git_submodule, GitSubmodulePointerDeleter>{sm};
}

} // namespace Git

#include "moc_submodule.cpp"
