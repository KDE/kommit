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
#include "libkommit_global.h"
#include "oid.h"
#include "repository.h"
#include "strarray.h"

namespace Git
{

namespace FetchCallbacks
{

struct FetchBridge {
    Fetch *observer;
    Repository *manager;
};

int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    auto bridge = reinterpret_cast<FetchBridge *>(data);

    auto ref = bridge->manager->references()->findByName(QString{refname});
    auto oidA = QSharedPointer<Oid>{new Oid{a}};
    auto oidB = QSharedPointer<Oid>{new Oid{b}};

    Q_EMIT bridge->observer->updateRef(ref, oidA, oidB);
    return 0;
}

int git_helper_sideband_progress_cb(const char *str, int len, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    Q_EMIT bridge->observer->message(QString::fromUtf8(str, len));

    return 0;
}

int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    static_assert(sizeof(git_indexer_progress) == sizeof(FetchTransferStat));

    auto stat = reinterpret_cast<const FetchTransferStat *>(stats);

    Q_EMIT bridge->observer->transferProgress(stat);
    return 0;
}

int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    Credential cred;

    cred.setUsername(username_from_url);
    cred.setAllowedTypes(static_cast<Credential::AllowedTypes>(allowed_types));

    Q_EMIT bridge->observer->credentialRequeted(QString{url}, &cred);

    git_credential_userpass_plaintext_new(out, cred.username().toLocal8Bit().data(), cred.password().toLocal8Bit().data());

    return 0;
}

int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    PackProgress p{stage, current, total};
    Q_EMIT bridge->observer->packProgress(&p);
    return 0;
}

int git_helper_transport_cb(git_transport **out, git_remote *owner, void *param)
{
    Q_UNUSED(out)
    Q_UNUSED(owner)
    Q_UNUSED(param)

    return 0;
}

int git_helper_transport_certificate_check(git_cert *cert, int valid, const char *host, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (Q_UNLIKELY(!bridge))
        return 0;

    Certificate c{cert, static_cast<bool>(valid), QString{host}};

    bool b{false};

    Q_EMIT bridge->observer->certificateCheck(&c, &b);

    return b ? 0 : -1;
}

}

class FetchPrivate
{
    Fetch *q_ptr;
    Q_DECLARE_PUBLIC(Fetch)

public:
    FetchPrivate(Fetch *parent, Repository *repo);

    Repository *repo;
    int depth;
    Fetch::Prune prune;
    Fetch::DownloadTags downloadTags;
    Fetch::Redirect redirect;
    QSharedPointer<Remote> remote;
    QSharedPointer<Branch> branch;
    QScopedPointer<FetchCallbacks::FetchBridge> bridge;
    QStringList customHeaders;
};

FetchPrivate::FetchPrivate(Fetch *parent, Repository *repo)
    : q_ptr{parent}
    , repo{repo}
{
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

void Fetch::run()
{
    Q_D(Fetch);
    git_fetch_options opts = GIT_FETCH_OPTIONS_INIT;

    // set callbacks
    opts.callbacks.update_tips = &FetchCallbacks::git_helper_update_tips_cb;
    opts.callbacks.sideband_progress = &FetchCallbacks::git_helper_sideband_progress_cb;
    opts.callbacks.transfer_progress = &FetchCallbacks::git_helper_transfer_progress_cb;
    opts.callbacks.credentials = &FetchCallbacks::git_helper_credentials_cb;
    opts.callbacks.pack_progress = &FetchCallbacks::git_helper_packbuilder_progress;

    opts.callbacks.transport = &FetchCallbacks::git_helper_transport_cb;
    opts.callbacks.certificate_check = &FetchCallbacks::git_helper_transport_certificate_check;

    // set variables
    if (d->depth)
        opts.depth = d->depth;

    opts.download_tags = static_cast<git_remote_autotag_option_t>(d->downloadTags);
    opts.follow_redirects = static_cast<git_remote_redirect_t>(d->redirect);
    opts.prune = static_cast<git_fetch_prune_t>(d->prune);

    if (d->bridge.isNull())
        d->bridge.reset(new FetchCallbacks::FetchBridge{this, d->repo});

    opts.callbacks.payload = d->bridge.data();

    if (!d->branch.isNull()) {
        StrArray refSpecs{1};
        refSpecs.add(d->branch->refName());
        git_remote_fetch(d->remote->remotePtr(), refSpecs, &opts, "fetch");
    } else {
        git_remote_fetch(d->remote->remotePtr(), NULL, &opts, "fetch");
    }
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

}
