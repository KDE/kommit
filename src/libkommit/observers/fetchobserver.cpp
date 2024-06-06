/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchobserver.h"
#include <git2/oid.h>

namespace Git
{

namespace FetchObserverCallbacks
{

int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data)
{
    auto observer = reinterpret_cast<Git::FetchObserver *>(data);

    return 0;
}

int git_helper_sideband_progress_cb(const char *str, int len, void *payload)
{
    auto observer = reinterpret_cast<Git::FetchObserver *>(payload);

    if (!observer)
        return 0;

    Q_EMIT observer->message(QString::fromLocal8Bit(str, len));

    return 0;
}

int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload)
{
    auto observer = reinterpret_cast<Git::FetchObserver *>(payload);

    if (!observer)
        return 0;

    observer->setTotalObjects(stats->total_objects);
    observer->setIndexedObjects(stats->indexed_objects);
    observer->setReceivedObjects(stats->received_objects);
    observer->setLocalObjects(stats->local_objects);
    observer->setTotalDeltas(stats->total_deltas);
    observer->setIndexedDeltas(stats->indexed_deltas);
    return 0;
}

int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    auto observer = reinterpret_cast<Git::FetchObserver *>(payload);

    if (!observer)
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

    Q_EMIT observer->credentialRequeted(QString{url}, &cred);

    git_credential_userpass_plaintext_new(out, observer->username().toLocal8Bit().data(), observer->password().toLocal8Bit().data());
    //    git_credential_username_new(out, observer->username().toLocal8Bit().data());
    return 0;
}

int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload)
{
    return 0;
}

int git_helper_transport_cb(git_transport **out, git_remote *owner, void *param)
{
    return 0;
}

}

FetchObserver::FetchObserver(QObject *parent)
    : QObject{parent}
{
}

unsigned int FetchObserver::totalObjects() const
{
    return mTotalObjects;
}

void FetchObserver::setTotalObjects(unsigned int totalObjects)
{
    if (mTotalObjects == totalObjects)
        return;
    mTotalObjects = totalObjects;
    Q_EMIT totalObjectsChanged(totalObjects);
}

unsigned int FetchObserver::indexedObjects() const
{
    return mIndexedObjects;
}

void FetchObserver::setIndexedObjects(unsigned int indexedObjects)
{
    if (mIndexedObjects == indexedObjects)
        return;
    mIndexedObjects = indexedObjects;
    Q_EMIT indexedObjectsChanged();
}

unsigned int FetchObserver::receivedObjects() const
{
    return mReceivedObjects;
}

void FetchObserver::setReceivedObjects(unsigned int receivedObjects)
{
    if (mReceivedObjects == receivedObjects)
        return;
    mReceivedObjects = receivedObjects;
    Q_EMIT receivedObjectsChanged(receivedObjects);
}

unsigned int FetchObserver::localObjects() const
{
    return mLocalObjects;
}

void FetchObserver::setLocalObjects(unsigned int localObjects)
{
    if (mLocalObjects == localObjects)
        return;
    mLocalObjects = localObjects;
    Q_EMIT localObjectsChanged();
}

unsigned int FetchObserver::totalDeltas() const
{
    return mTotalDeltas;
}

void FetchObserver::setTotalDeltas(unsigned int totalDeltas)
{
    if (mTotalDeltas == totalDeltas)
        return;
    mTotalDeltas = totalDeltas;
    Q_EMIT totalDeltasChanged();
}

unsigned int FetchObserver::indexedDeltas() const
{
    return mIndexedDeltas;
}

void FetchObserver::setIndexedDeltas(unsigned int indexedDeltas)
{
    if (mIndexedDeltas == indexedDeltas)
        return;
    mIndexedDeltas = indexedDeltas;
    Q_EMIT indexedDeltasChanged();
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

QString Credential::username() const
{
    return mUsername;
}

void Credential::setUsername(const QString &username)
{
    mUsername = username;
}

QString Credential::password() const
{
    return mPassword;
}

void Credential::setPassword(const QString &password)
{
    mPassword = password;
}

Credential::AllowedTypes Credential::allowedTypes() const
{
    return mAllowedTypes;
}

void Credential::setAllowedTypes(const AllowedTypes &allowedTypes)
{
    mAllowedTypes = allowedTypes;
}
}

#include "moc_fetchobserver.cpp"
