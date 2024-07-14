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
#include <git2/submodule.h>

#include <gitmanager.h>

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
    SubmodulePrivate(Submodule *parent, git_repository *repo = nullptr);

    git_repository *repo{nullptr};
    QString name;
    QString url;
    QString path;
    QString refName;
    QString branch;
    QScopedPointer<git_submodule, GitSubmodulePointerDeleter> find() const;
};

Submodule::Submodule(git_submodule *submodule, git_repository *repo)
    : d_ptr{new SubmodulePrivate{this, repo}}
{
    Q_D(Submodule);

    d->name = QString{git_submodule_name(submodule)};
    d->path = QString{git_submodule_path(submodule)};
    d->url = QString{git_submodule_url(submodule)};

    auto headId = git_submodule_head_id(submodule);
    d->refName = QString{git_oid_tostr_s(headId)};

    if (!d->repo)
        d->repo = git_submodule_owner(submodule);
    d->branch = git_submodule_branch(submodule);
}

Submodule::~Submodule()
{
    Q_D(Submodule);
    // git_submodule_free(submodule);
    delete d;
}

const QString &Submodule::path() const
{
    Q_D(const Submodule);
    return d->path;
}

const QString &Submodule::refName() const
{
    Q_D(const Submodule);
    return d->refName;
}

QString Submodule::url() const
{
    Q_D(const Submodule);
    return d->url;
}

void Submodule::setUrl(const QString &newUrl)
{
    Q_D(Submodule);
    d->url = newUrl;
    BEGIN;
    STEP git_submodule_set_url(d->repo, toConstChars(d->name), toConstChars(newUrl));
}

QString Submodule::name() const
{
    Q_D(const Submodule);
    return d->name;
}

QString Submodule::branch()
{
    Q_D(Submodule);
    return d->branch;
}

Submodule::StatusFlags Submodule::status() const
{
    Q_D(const Submodule);
    unsigned int status;

    if (git_submodule_status(&status, d->repo, d->name.toLocal8Bit().constData(), GIT_SUBMODULE_IGNORE_UNSPECIFIED)) {
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

bool Submodule::hasModifiedFiles() const
{
    Q_D(const Submodule);
    unsigned int status;

    BEGIN
    STEP git_submodule_status(&status, d->repo, d->name.toLocal8Bit().constData(), GIT_SUBMODULE_IGNORE_UNSPECIFIED);
    END;

    RETURN_COND(GIT_SUBMODULE_STATUS_IS_WD_DIRTY(status), false);
}

QSharedPointer<Oid> Submodule::headId()
{
    Q_D(Submodule);
    auto submodule = d->find();
    return QSharedPointer<Oid>{new Oid{git_submodule_head_id(submodule.get())}};
}

QSharedPointer<Oid> Submodule::indexId()
{
    Q_D(Submodule);
    auto submodule = d->find();
    return QSharedPointer<Oid>{new Oid{git_submodule_index_id(submodule.get())}};
}

QSharedPointer<Oid> Submodule::workingDirectoryId()
{
    Q_D(Submodule);
    auto submodule = d->find();
    return QSharedPointer<Oid>{new Oid{git_submodule_wd_id(submodule.get())}};
}

bool Submodule::sync() const
{
    Q_D(const Submodule);
    auto submodule = d->find();

    if (Q_UNLIKELY(!submodule))
        return false;

    return !git_submodule_sync(submodule.get());
}

bool Submodule::reload(bool force) const
{
    Q_D(const Submodule);
    auto submodule = d->find();
    if (Q_UNLIKELY(!submodule))
        return false;
    return !git_submodule_reload(submodule.get(), force);
}

Manager *Submodule::open() const
{
    Q_D(const Submodule);
    auto submodule = d->find();
    git_repository *repo;
    if (git_submodule_open(&repo, submodule.get()))
        return nullptr;

    return new Git::Manager{repo};
}

bool Submodule::update(const FetchOptions &opts, FetchObserver *observer)
{
    Q_D(Submodule);

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

QScopedPointer<git_submodule, GitSubmodulePointerDeleter> SubmodulePrivate::find() const
{
    git_submodule *sm;

    git_submodule_lookup(&sm, repo, name.toUtf8().data());

    return QScopedPointer<git_submodule, GitSubmodulePointerDeleter>{sm};
}

} // namespace Git

#include "moc_submodule.cpp"
