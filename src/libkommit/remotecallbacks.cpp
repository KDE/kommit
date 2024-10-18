/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotecallbacks.h"

#include "caches/referencecache.h"
#include "certificate.h"
#include "credential.h"
#include "oid.h"
#include "repository.h"
#include "remoteconnectionlistener.h"

namespace Git
{

namespace Callbacks
{

struct FetchBridge {
    RemoteCallbacks *fetch;
    Repository *manager;
};

int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    auto bridge = reinterpret_cast<FetchBridge *>(data);

    auto ref = bridge->manager->references()->findByName(QString{refname});
    auto oidA = QSharedPointer<Oid>{new Oid{a}};
    auto oidB = QSharedPointer<Oid>{new Oid{b}};

    Q_EMIT bridge->fetch->updateRef(ref, oidA, oidB);
    return 0;
}

int git_helper_sideband_progress_cb(const char *str, int len, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    Q_EMIT bridge->fetch->message(QString::fromUtf8(str, len));

    return 0;
}

int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    static_assert(sizeof(git_indexer_progress) == sizeof(FetchTransferStat));

    auto stat = reinterpret_cast<const FetchTransferStat *>(stats);

    Q_EMIT bridge->fetch->transferProgress(stat);
    return 0;
}

int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return GIT_EUSER;

    auto m = bridge->manager->remoteConnectionListener();
    if (!m)
        return GIT_PASSTHROUGH;

    Credential cred;

    cred.setUsername(username_from_url);
    cred.setAllowedTypes(static_cast<Credential::AllowedTypes>(allowed_types));

    bool accept{false};

    m->credentialRequeted(QString{url}, &cred, &accept);

    if (!accept)
        return GIT_PASSTHROUGH;

    git_credential_userpass_plaintext_new(out, cred.username().toLocal8Bit().data(), cred.password().toLocal8Bit().data());

    return 0;
}

int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    PackProgress p{stage, current, total};
    Q_EMIT bridge->fetch->packProgress(&p);
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

    auto m = bridge->manager->remoteConnectionListener();
    if (!m)
        return GIT_PASSTHROUGH;

    Certificate c{cert, static_cast<bool>(valid), QString{host}};

    bool b{true};

    m->certificateCheck(&c, &b);

    return b ? 0 : -1;
}

int git_helper_remote_ready_cb(git_remote *remote, int direction, void *payload)
{
    return 0;
}

} // name RemoteCallbacks

RemoteCallbacks::RemoteCallbacks(QObject *parent)
    : QObject{parent}
{
}

void RemoteCallbacks::apply(git_remote_callbacks *callbacks, Repository *repo)
{
    auto bridge = new Callbacks::FetchBridge;
    bridge->manager = repo;
    bridge->fetch = this;
    callbacks->update_tips = &Callbacks::git_helper_update_tips_cb;
    callbacks->sideband_progress = &Callbacks::git_helper_sideband_progress_cb;
    callbacks->transfer_progress = &Callbacks::git_helper_transfer_progress_cb;
    callbacks->credentials = &Callbacks::git_helper_credentials_cb;
    callbacks->pack_progress = &Callbacks::git_helper_packbuilder_progress;
    callbacks->transport = &Callbacks::git_helper_transport_cb;
    callbacks->certificate_check = &Callbacks::git_helper_transport_certificate_check;
    callbacks->remote_ready = &Callbacks::git_helper_remote_ready_cb;
    callbacks->payload = bridge;
}

} // namespace Git
