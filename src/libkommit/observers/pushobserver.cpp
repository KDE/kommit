#include "pushobserver.h"

namespace Git
{

namespace PushCallbacks
{

int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload)
{
    auto observer = reinterpret_cast<PushObserver *>(payload);
    if (!observer)
        return 0;

    observer->setPackProgressValue(current);
    observer->setPackProgressTotal(total);
    return 0;
}

int git_helper_push_transfer_progress_cb(unsigned int current, unsigned int total, size_t bytes, void *payload)
{
    auto observer = reinterpret_cast<PushObserver *>(payload);
    if (!observer)
        return 0;

    observer->setTransferProgressValue(current);
    observer->setTransferProgressTotal(total);
    return 0;
}

int git_helper_credential_acquire_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload)
{
    auto observer = reinterpret_cast<PushObserver *>(payload);
    if (!observer)
        return 0;

    return 0;
}

int git_helper_transport_certificate_check_cb(git_cert *cert, int valid, const char *host, void *payload)
{
    auto observer = reinterpret_cast<PushObserver *>(payload);
    if (!observer)
        return 0;

    return 0;
}

}

PushObserver::PushObserver(QObject *parent)
{
}

unsigned int PushObserver::packProgressValue() const
{
    return mPackProgressValue;
}

void PushObserver::setPackProgressValue(unsigned int packProgressValue)
{
    if (mPackProgressValue == packProgressValue)
        return;
    mPackProgressValue = packProgressValue;
    emit packProgressValueChanged();
}

unsigned int PushObserver::packProgressTotal() const
{
    return mPackProgressTotal;
}

void PushObserver::setPackProgressTotal(unsigned int packProgressTotal)
{
    if (mPackProgressTotal == packProgressTotal)
        return;
    mPackProgressTotal = packProgressTotal;
    emit packProgressTotalChanged();
}

unsigned int PushObserver::transferProgressValue() const
{
    return mTransferProgressValue;
}

void PushObserver::setTransferProgressValue(unsigned int transferProgressValue)
{
    if (mTransferProgressValue == transferProgressValue)
        return;
    mTransferProgressValue = transferProgressValue;
    emit transferProgressValueChanged();
}

unsigned int PushObserver::transferProgressTotal() const
{
    return mTransferProgressTotal;
}

void PushObserver::setTransferProgressTotal(unsigned int transferProgressTotal)
{
    if (mTransferProgressTotal == transferProgressTotal)
        return;
    mTransferProgressTotal = transferProgressTotal;
    emit transferProgressTotalChanged();
}

}
