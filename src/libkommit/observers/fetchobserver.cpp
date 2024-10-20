/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchobserver.h"
#include "caches/referencecache.h"
#include "credential.h"
#include "entities/oid.h"
#include "repository.h"

#include <git2/oid.h>

namespace Git
{

struct FetchObserverBridge {
    FetchObserver *observer;
    Repository *manager;
};
namespace FetchObserverCallbacks
{

int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    auto bridge = reinterpret_cast<Git::FetchObserverBridge *>(data);

    auto ref = bridge->manager->references()->findByName(QString{refname});
    Oid oidA{a};
    Oid oidB{b};

    Q_EMIT bridge->observer->updateRef(ref, oidA, oidB);
    return 0;
}

int git_helper_sideband_progress_cb(const char *str, int len, void *payload)
{
    auto bridge = reinterpret_cast<Git::FetchObserverBridge *>(payload);

    if (!bridge)
        return 0;

    Q_EMIT bridge->observer->message(QString::fromLocal8Bit(str, len));

    return 0;
}

int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload)
{
    auto bridge = reinterpret_cast<Git::FetchObserverBridge *>(payload);

    if (!bridge)
        return 0;

    static_assert(sizeof(git_indexer_progress) == sizeof(FetchTransferStat));

    auto stat = reinterpret_cast<const FetchTransferStat *>(stats);

    // FetchTransferStat stat{stats->total_objects,
    //                        stats->indexed_objects,
    //                        stats->received_objects,
    //                        stats->local_objects,
    //                        stats->total_deltas,
    //                        stats->indexed_deltas,
    //                        stats->received_bytes};
    Q_EMIT bridge->observer->transferProgress(stat);
    return 0;
}

int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    auto bridge = reinterpret_cast<Git::FetchObserverBridge *>(payload);

    if (!bridge)
        return 0;

    Credential cred;

    cred.setUsername(username_from_url);

    Credential::AllowedTypes credAllowedType{};
    if (allowed_types & GIT_CREDENTIAL_USERPASS_PLAINTEXT)
        credAllowedType |= Credential::AllowedType::UserpassPlaintext;
    if (allowed_types & GIT_CREDENTIAL_SSH_KEY)
        credAllowedType |= Credential::AllowedType::SshKey;
    if (allowed_types & GIT_CREDENTIAL_SSH_CUSTOM)
        credAllowedType |= Credential::AllowedType::SshCustom;
    if (allowed_types & GIT_CREDENTIAL_DEFAULT)
        credAllowedType |= Credential::AllowedType::Defaut;
    if (allowed_types & GIT_CREDENTIAL_SSH_INTERACTIVE)
        credAllowedType |= Credential::AllowedType::SshInteractive;
    if (allowed_types & GIT_CREDENTIAL_USERNAME)
        credAllowedType |= Credential::AllowedType::Username;
    if (allowed_types & GIT_CREDENTIAL_SSH_MEMORY)
        credAllowedType |= Credential::AllowedType::SshMemory;

    cred.setAllowedTypes(credAllowedType);

    Q_EMIT bridge->observer->credentialRequeted(QString{url}, &cred);

    git_credential_userpass_plaintext_new(out, cred.username().toLocal8Bit().data(), cred.password().toLocal8Bit().data());

    return 0;
}

int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload)
{
    auto bridge = reinterpret_cast<Git::FetchObserverBridge *>(payload);

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
    Q_UNUSED(valid)
    Q_UNUSED(cert)
    Q_UNUSED(host)
    Q_UNUSED(payload)
    return 0;
}
}

FetchObserver::FetchObserver(Repository *parent)
    : QObject{parent}
    , mManager{parent}
{
}

void FetchObserver::applyOfFetchOptions(git_fetch_options *opts)
{
    opts->callbacks.update_tips = &FetchObserverCallbacks::git_helper_update_tips_cb;
    opts->callbacks.sideband_progress = &FetchObserverCallbacks::git_helper_sideband_progress_cb;
    opts->callbacks.transfer_progress = &FetchObserverCallbacks::git_helper_transfer_progress_cb;
    opts->callbacks.credentials = &FetchObserverCallbacks::git_helper_credentials_cb;
    opts->callbacks.pack_progress = &FetchObserverCallbacks::git_helper_packbuilder_progress;
    opts->callbacks.transport = &FetchObserverCallbacks::git_helper_transport_cb;
    opts->callbacks.certificate_check = &FetchObserverCallbacks::git_helper_transport_certificate_check;

    if (!mBridge)
        mBridge = new FetchObserverBridge;

    mBridge->manager = mManager;
    mBridge->observer = this;
    opts->callbacks.payload = mBridge;
}

void FetchObserver::setCredential(const QString &username, const QString &password)
{
    mUsername = username;
    mPassword = password;
}

QString FetchObserver::username() const
{
    return mUsername;
}

QString FetchObserver::password() const
{
    return mPassword;
}
}

#include "moc_fetchobserver.cpp"
