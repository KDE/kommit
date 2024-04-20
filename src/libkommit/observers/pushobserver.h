#pragma once

#include <QObject>

#include <git2/cert.h>
#include <git2/credential.h>

namespace Git
{

namespace PushCallbacks
{
int git_helper_packbuilder_progress(int stage, uint32_t current, uint32_t total, void *payload);
int git_helper_push_transfer_progress_cb(unsigned int current, unsigned int total, size_t bytes, void *payload);
int git_helper_credential_acquire_cb(git_credential **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload);
int git_helper_transport_certificate_check_cb(git_cert *cert, int valid, const char *host, void *payload);
}

class PushObserver : public QObject
{
    Q_OBJECT
public:
    explicit PushObserver(QObject *parent = nullptr);

    Q_REQUIRED_RESULT unsigned int packProgressValue() const;
    void setPackProgressValue(unsigned int packProgressValue);
    Q_REQUIRED_RESULT unsigned int packProgressTotal() const;
    void setPackProgressTotal(unsigned int packProgressTotal);
    Q_REQUIRED_RESULT unsigned int transferProgressValue() const;
    void setTransferProgressValue(unsigned int transferProgressValue);
    Q_REQUIRED_RESULT unsigned int transferProgressTotal() const;
    void setTransferProgressTotal(unsigned int transferProgressTotal);

signals:
    void packProgressValueChanged();
    void packProgressTotalChanged();
    void transferProgressValueChanged();
    void transferProgressTotalChanged();

private:
    unsigned int mPackProgressValue;
    unsigned int mPackProgressTotal;
    unsigned int mTransferProgressValue;
    unsigned int mTransferProgressTotal;
    Q_PROPERTY(unsigned int packProgressValue READ packProgressValue WRITE setPackProgressValue NOTIFY packProgressValueChanged FINAL)
    Q_PROPERTY(unsigned int packProgressTotal READ packProgressTotal WRITE setPackProgressTotal NOTIFY packProgressTotalChanged FINAL)
    Q_PROPERTY(unsigned int transferProgressValue READ transferProgressValue WRITE setTransferProgressValue NOTIFY transferProgressValueChanged FINAL)
    Q_PROPERTY(unsigned int transferProgressTotal READ transferProgressTotal WRITE setTransferProgressTotal NOTIFY transferProgressTotalChanged FINAL)
};

}
