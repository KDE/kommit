/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>

#include <git2/credential.h>
#include <git2/indexer.h>
#include <git2/types.h>

int git_helper_update_tips_cb(const char *refname, const git_oid *a, const git_oid *b, void *data);
int git_helper_sideband_progress_cb(const char *str, int len, void *payload);
int git_helper_transfer_progress_cb(const git_indexer_progress *stats, void *payload);
int git_helper_credentials_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload);

namespace Git
{

class LIBKOMMIT_EXPORT FetchObserver : public QObject
{
    Q_OBJECT

    Q_PROPERTY(unsigned int totalObjects READ totalObjects WRITE setTotalObjects NOTIFY totalObjectsChanged FINAL)
    Q_PROPERTY(unsigned int indexedObjects READ indexedObjects WRITE setIndexedObjects NOTIFY indexedObjectsChanged FINAL)
    Q_PROPERTY(unsigned int receivedObjects READ receivedObjects WRITE setReceivedObjects NOTIFY receivedObjectsChanged FINAL)
    Q_PROPERTY(unsigned int localObjects READ localObjects WRITE setLocalObjects NOTIFY localObjectsChanged FINAL)
    Q_PROPERTY(unsigned int totalDeltas READ totalDeltas WRITE setTotalDeltas NOTIFY totalDeltasChanged FINAL)
    Q_PROPERTY(unsigned int indexedDeltas READ indexedDeltas WRITE setIndexedDeltas NOTIFY indexedDeltasChanged FINAL)

public:
    FetchObserver(QObject *parent = nullptr);

    Q_REQUIRED_RESULT unsigned int totalObjects() const;
    void setTotalObjects(unsigned int totalObjects);

    Q_REQUIRED_RESULT unsigned int indexedObjects() const;
    void setIndexedObjects(unsigned int indexedObjects);

    Q_REQUIRED_RESULT unsigned int receivedObjects() const;
    void setReceivedObjects(unsigned int receivedObjects);

    Q_REQUIRED_RESULT unsigned int localObjects() const;
    void setLocalObjects(unsigned int localObjects);

    Q_REQUIRED_RESULT unsigned int totalDeltas() const;
    void setTotalDeltas(unsigned int totalDeltas);

    Q_REQUIRED_RESULT unsigned int indexedDeltas() const;
    void setIndexedDeltas(unsigned int indexedDeltas);

    void setCredential(const QString &username, const QString &password);
    Q_REQUIRED_RESULT QString username() const;
    Q_REQUIRED_RESULT QString password() const;

signals:
    void totalObjectsChanged();
    void indexedObjectsChanged();
    void receivedObjectsChanged();
    void localObjectsChanged();
    void totalDeltasChanged();
    void indexedDeltasChanged();

    void message(const QString &message);
    void credentialRequeted();

private:
    unsigned int mTotalObjects{};
    unsigned int mIndexedObjects{};
    unsigned int mReceivedObjects{};
    unsigned int mLocalObjects{};
    unsigned int mTotalDeltas{};
    unsigned int mIndexedDeltas{};

    QString mUsername;
    QString mPassword;
};

}
