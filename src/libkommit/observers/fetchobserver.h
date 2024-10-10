/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>

#include <git2/credential.h>
#include <git2/indexer.h>
#include <git2/remote.h>
#include <git2/types.h>

namespace Git
{

class Repository;
class Oid;
class Reference;
class Credential;

/*namespace FetchObserverCallbacks
{
int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data);
int git_helper_sideband_progress_cb(const char *str, int len, void *payload);
int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload);
int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload);
int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload);
int git_helper_transport_cb(git_transport **out, git_remote *owner, void *param);
}*/

struct FetchObserverBridge;
struct FetchTransferStat {
    unsigned int totalObjects;
    unsigned int indexedObjects;
    unsigned int receivedObjects;
    unsigned int localObjects;
    unsigned int totalDeltas;
    unsigned int indexedDeltas;
    size_t receivedBytes;
};
struct PackProgress {
    int stage;
    uint32_t current;
    uint32_t total;
};

class LIBKOMMIT_EXPORT FetchObserver : public QObject
{
    Q_OBJECT

public:
    explicit FetchObserver(Repository *parent);

    void applyOfFetchOptions(git_fetch_options *opts);

    void setCredential(const QString &username, const QString &password);
    [[nodiscard]] QString username() const;
    [[nodiscard]] QString password() const;

Q_SIGNALS:
    void message(const QString &message);
    void credentialRequeted(const QString &url, Credential *cred);
    void transferProgress(const FetchTransferStat *stat);
    void packProgress(const PackProgress *p);
    void updateRef(QSharedPointer<Reference> reference, QSharedPointer<Oid> a, QSharedPointer<Oid> b);
    void finished();

private:
    QString mUsername;
    QString mPassword;

    Repository *mManager;
    FetchObserverBridge *mBridge{nullptr};
};

}
