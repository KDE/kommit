/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchoptions.h"
#include "caches/referencecache.h"
#include "certificate.h"
#include "credential.h"
#include "reference.h"
#include "remoteconnectionlistener.h"
#include "repository.h"

#include <QMetaMethod>

#include <Kommit/Oid>

namespace Git
{

namespace FetchOptionsCallbacks
{

class FetchBridge
{
public:
    Repository *manager;
    QObject *parent;

    bool messageIndexFound{false};
    bool transferProgressIndexFound{false};
    bool packProgressIndexFound{false};
    bool updateRefIndexFound{false};

    QMetaMethod messageIndexSignal;
    QMetaMethod transferProgressIndexSignal;
    QMetaMethod packProgressIndexSignal;
    QMetaMethod updateRefIndexSignal;

    void findSignals()
    {
        auto metaObject = parent->metaObject();

        auto messageIndex = metaObject->indexOfSignal("message");
        auto transferProgressIndex = metaObject->indexOfSignal("transferProgress");
        auto packProgressIndex = metaObject->indexOfSignal("packProgress");
        auto updateRefIndex = metaObject->indexOfSignal("updateRef");

        messageIndexFound = messageIndex != -1;
        transferProgressIndexFound = transferProgressIndex != -1;
        packProgressIndexFound = packProgressIndex != -1;
        updateRefIndexFound = updateRefIndex != -1;

        if (messageIndex != -1)
            messageIndexSignal = metaObject->method(messageIndex);
        if (transferProgressIndex != -1)
            transferProgressIndexSignal = metaObject->method(transferProgressIndex);
        if (packProgressIndex != -1)
            packProgressIndexSignal = metaObject->method(packProgressIndex);
        if (updateRefIndex != -1)
            updateRefIndexSignal = metaObject->method(updateRefIndex);
    }
};

int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    auto bridge = reinterpret_cast<FetchBridge *>(data);

    auto ref = bridge->manager->references()->findByName(QString{refname});
    Oid oidA{a};
    Oid oidB{b};

    if (Q_LIKELY(bridge->updateRefIndexFound))
        bridge->updateRefIndexSignal.invoke(bridge->parent, ref.data(), oidA, oidB);
    return 0;
}

int git_helper_sideband_progress_cb(const char *str, int len, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    if (Q_LIKELY(bridge->messageIndexFound))
        bridge->messageIndexSignal.invoke(bridge->parent, QString::fromUtf8(str, len));

    return 0;
}

int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload)
{
    auto bridge = reinterpret_cast<FetchBridge *>(payload);

    if (!bridge)
        return 0;

    static_assert(sizeof(git_indexer_progress) == sizeof(FetchTransferStat));

    auto stat = reinterpret_cast<const FetchTransferStat *>(stats);

    if (Q_LIKELY(bridge->transferProgressIndexFound))
        bridge->transferProgressIndexSignal.invoke(bridge->parent, stat);
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
    if (Q_LIKELY(bridge->packProgressIndexFound))
        bridge->packProgressIndexSignal.invoke(bridge->parent, &p);
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

FetchOptions::FetchOptions(QObject *parent)
{
}

void FetchOptions::applyToFetchOptions(git_fetch_options *opts) const
{
    opts->download_tags = static_cast<git_remote_autotag_option_t>(downloadTags);
    opts->prune = static_cast<git_fetch_prune_t>(prune);
}

void FetchOptions::apply(git_remote_callbacks *callbacks, Repository *repo)
{
    auto bridge = new FetchOptionsCallbacks::FetchBridge;
    bridge->manager = repo;
    // bridge->parent = this;
    //  bridge->parent = ... //TODO
    bridge->findSignals();

    callbacks->update_tips = &FetchOptionsCallbacks::git_helper_update_tips_cb;
    callbacks->sideband_progress = &FetchOptionsCallbacks::git_helper_sideband_progress_cb;
    callbacks->transfer_progress = &FetchOptionsCallbacks::git_helper_transfer_progress_cb;
    callbacks->credentials = &FetchOptionsCallbacks::git_helper_credentials_cb;
    callbacks->pack_progress = &FetchOptionsCallbacks::git_helper_packbuilder_progress;
    callbacks->transport = &FetchOptionsCallbacks::git_helper_transport_cb;
    callbacks->certificate_check = &FetchOptionsCallbacks::git_helper_transport_certificate_check;
    callbacks->remote_ready = &FetchOptionsCallbacks::git_helper_remote_ready_cb;
    callbacks->payload = bridge;
}

}
