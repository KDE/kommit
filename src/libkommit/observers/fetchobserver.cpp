#include "fetchobserver.h"
#include <git2/oid.h>

namespace Git
{

FetchObserver::FetchObserver(QObject *parent)
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
    emit totalObjectsChanged();
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
    emit indexedObjectsChanged();
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
    emit receivedObjectsChanged();
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
    emit localObjectsChanged();
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
    emit totalDeltasChanged();
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
    emit indexedDeltasChanged();
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

    QString msg;
    Q_EMIT observer->message(msg);

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

    emit observer->credentialRequeted();

    git_credential_userpass_plaintext_new(out, observer->username().toLocal8Bit().data(), observer->password().toLocal8Bit().data());
    //    git_credential_username_new(out, observer->username().toLocal8Bit().data());
    return 0;
}
