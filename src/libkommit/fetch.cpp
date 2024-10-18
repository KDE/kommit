/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetch.h"

#include "caches/referencecache.h"
#include "certificate.h"
#include "credential.h"
#include "entities/branch.h"
#include "entities/remote.h"
#include "gitglobal_p.h"
#include "libkommit_global.h"
#include "oid.h"
#include "remotecallbacks.h"
#include "repository.h"
#include "strarray.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

namespace Git
{

class FetchPrivate
{
    Fetch *q_ptr;
    Q_DECLARE_PUBLIC(Fetch)

public:
    FetchPrivate(Fetch *parent, Repository *repo);

    Repository *repo;
    int depth{-1};
    Fetch::Prune prune{Fetch::Prune::PruneUnspecified};
    Fetch::DownloadTags downloadTags{Fetch::DownloadTags::Unspecified};
    Fetch::Redirect redirect{Fetch::Redirect::All};
    QSharedPointer<Remote> remote;
    QSharedPointer<Branch> branch;
    QStringList customHeaders;
    Fetch::AcceptCertificate acceptCertificate{Fetch::AcceptCertificate::OnlyValid};
    RemoteCallbacks callbacks;

    int run();
};

FetchPrivate::FetchPrivate(Fetch *parent, Repository *repo)
    : q_ptr{parent}
    , repo{repo}
{
}

int FetchPrivate::run()
{
    Q_Q(Fetch);

    if (remote.isNull())
        return -1;

    if (!remote->isConnected() && !remote->connect(Git::Remote::Direction::Fetch, &callbacks))
        return -1;

    git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;
    git_fetch_options_init(&opts, GIT_FETCH_OPTIONS_VERSION);

    callbacks.apply(&opts.callbacks, repo);

    // set variables
    if (depth > -1)
        opts.depth = depth;

    opts.download_tags = static_cast<git_remote_autotag_option_t>(downloadTags);
    opts.follow_redirects = static_cast<git_remote_redirect_t>(redirect);
    opts.prune = static_cast<git_fetch_prune_t>(prune);

    int ret;
    if (!branch.isNull()) {
        StrArray refSpecs{1};
        refSpecs.add(branch->refName());
        ret = SequenceRunner::runSingle(git_remote_fetch, remote->remotePtr(), refSpecs, &opts, "fetch");
    } else {
        ret = SequenceRunner::runSingle(git_remote_fetch, remote->remotePtr(), (const git_strarray *)NULL, &opts, "fetch");
    }
    return ret;
}

Fetch::Fetch(Repository *repo, QObject *parent)
    : QObject{parent}
    , d_ptr{new FetchPrivate{this, repo}}
{
}

Fetch::~Fetch()
{
}

QSharedPointer<Remote> Fetch::remote() const
{
    Q_D(const Fetch);
    return d->remote;
}

void Fetch::setRemote(QSharedPointer<Remote> remote)
{
    Q_D(Fetch);
    d->remote = remote;
}

Fetch::Prune Fetch::prune() const
{
    Q_D(const Fetch);
    return d->prune;
}

void Fetch::setPrune(Prune prune)
{
    Q_D(Fetch);
    d->prune = prune;
}

Fetch::DownloadTags Fetch::downloadTags() const
{
    Q_D(const Fetch);
    return d->downloadTags;
}

void Fetch::setDownloadTags(DownloadTags downloadTags)
{
    Q_D(Fetch);
    d->downloadTags = downloadTags;
}

int Fetch::depth() const
{
    Q_D(const Fetch);
    return d->depth;
}

void Fetch::setDepth(int depth)
{
    Q_D(Fetch);
    d->depth = depth;
}

Fetch::Redirect Fetch::redirect() const
{
    Q_D(const Fetch);
    return d->redirect;
}

void Fetch::setRedirect(Redirect redirect)
{
    Q_D(Fetch);
    d->redirect = redirect;
}

bool Fetch::run()
{
    Q_D(Fetch);
    auto retCode = d->run();
    Q_EMIT finished(0 == retCode);
    return 0 == retCode;
}

void Fetch::runAsync()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QtConcurrent::run(this, &Fetch::run);
#else
    QtConcurrent::run(&Fetch::run, this);
#endif
}

QStringList Fetch::customHeaders() const
{
    Q_D(const Fetch);
    return d->customHeaders;
}

void Fetch::setCustomHeaders(const QStringList &customHeaders)
{
    Q_D(Fetch);
    d->customHeaders = customHeaders;
}

QSharedPointer<Branch> Fetch::branch() const
{
    Q_D(const Fetch);
    return d->branch;
}

void Fetch::setBranch(QSharedPointer<Branch> branch)
{
    Q_D(Fetch);
    d->branch = branch;
}

Fetch::AcceptCertificate Fetch::acceptCertificate() const
{
    Q_D(const Fetch);
    return d->acceptCertificate;
}

void Fetch::setAcceptCertificate(AcceptCertificate acceptCertificate)
{
    Q_D(Fetch);
    d->acceptCertificate = acceptCertificate;
}

const RemoteCallbacks *Fetch::remoteCallbacks() const
{
    Q_D(const Fetch);
    return &d->callbacks;
}

}
