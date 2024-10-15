#include "certificate.h"

#ifdef LIBKOMMIT_USE_OPENSSL
#include <openssl/x509.h>
#endif

#include <QDebug>

#define GET_X509_VAR(func, var)                                                                                                                                \
    do {                                                                                                                                                       \
        char buffer##var[256];                                                                                                                                 \
        X509_NAME_oneline(X509_get_subject_name(x509), buffer##var, sizeof(buffer##var));                                                                      \
        d->x509.var = QString::fromUtf8(buffer##var);                                                                                                          \
    } while (false)

namespace Git
{

class CertificatePrivate
{
    Certificate *q_ptr;
    Q_DECLARE_PUBLIC(Certificate)

public:
    CertificatePrivate(Certificate *parent);

    git_cert *cert;
    Certificate::Type type{Certificate::Type::None};
    bool isValid;
    QString host;
    Certificate::X509Data x509;
    Certificate::HostKeyData hostKey;

#ifdef LIBKOMMIT_USE_OPENSSL
    void readX509(X509 *x509)
    {
        char subjectBuffer[256];
        X509_NAME_oneline(X509_get_subject_name(x509), subjectBuffer, sizeof(subjectBuffer));
        this->x509.subject = QString::fromUtf8(subjectBuffer);

        // Get the issuer name
        char issuerBuffer[256];
        X509_NAME_oneline(X509_get_issuer_name(x509), issuerBuffer, sizeof(issuerBuffer));
        this->x509.issuer = QString::fromUtf8(issuerBuffer);

        // Get the serial number
        ASN1_INTEGER *serial = X509_get_serialNumber(x509);
        if (serial) {
            BIGNUM *bn = ASN1_INTEGER_to_BN(serial, nullptr);
            if (bn) {
                char *hex = BN_bn2hex(bn);
                if (hex) {
                    this->x509.serialNumber = QString::fromUtf8(hex);
                    OPENSSL_free(hex);
                }
                BN_free(bn);
            }
        }
    }
#endif
};

CertificatePrivate::CertificatePrivate(Certificate *parent)
    : q_ptr{parent}
{
}

Certificate::Certificate(git_cert *cert, bool valid, const QString &host)
    : d_ptr{new CertificatePrivate{this}}
{
    Q_D(Certificate);
    d->cert = cert;
    d->isValid = valid;
    d->host = host;

    if (cert) {
        d->type = static_cast<Type>(cert->cert_type);

        switch (d->type) {
        case Git::Certificate::Type::None:
            break;
        case Git::Certificate::Type::X509: {
#ifdef LIBKOMMIT_USE_OPENSSL
            git_cert_x509 *cert_x509 = (git_cert_x509 *)cert;
            X509 *x509 = (X509 *)(cert_x509->data);

            if (x509) {
                d->readX509(x509);
                // GET_X509_VAR(X509_get_subject_name, subject);
                // GET_X509_VAR(X509_get_issuer_name, issuer);
                // GET_X509_VAR(X509_get_serialNumber, serialNumber);
            }
#endif
            break;
        }
        case Git::Certificate::Type::HostkeyLibssh2: {
            git_cert_hostkey *cert_hostkey = (git_cert_hostkey *)cert;

            d->hostKey.hashType = static_cast<HostKeyHashType>(cert_hostkey->type);

            switch (cert_hostkey->type) {
            case GIT_CERT_SSH_MD5:
                d->hostKey.hash = QByteArray((char *)cert_hostkey->hash_md5, 16);
                break;
            case GIT_CERT_SSH_SHA1:
                d->hostKey.hash = QByteArray((char *)cert_hostkey->hash_sha1, 20);
                break;
            case GIT_CERT_SSH_SHA256:
                d->hostKey.hash = QByteArray((char *)cert_hostkey->hash_sha256, 32);
                break;
            case GIT_CERT_SSH_RAW:
                d->hostKey.hash = QByteArray((char *)cert_hostkey->hostkey, cert_hostkey->hostkey_len);
                break;
            }

            break;
        }
        case Git::Certificate::Type::Strarray:
            break;
        }
    }
}

Certificate::~Certificate()
{
}

Certificate::Type Certificate::type() const
{
    Q_D(const Certificate);
    return d->type;
}

void Certificate::setType(Type type)
{
    Q_D(Certificate);
    d->type = type;
}

bool Certificate::isValid() const
{
    Q_D(const Certificate);
    return d->isValid;
}

void Certificate::setIsValid(bool isValid)
{
    Q_D(Certificate);
    d->isValid = isValid;
}

QString Certificate::host() const
{
    Q_D(const Certificate);
    return d->host;
}

void Certificate::setHost(const QString &host)
{
    Q_D(Certificate);
    d->host = host;
}

const Certificate::X509Data &Certificate::x509() const
{
    Q_D(const Certificate);
    return d->x509;
}

const Certificate::HostKeyData &Certificate::hostKey() const
{
    Q_D(const Certificate);
    return d->hostKey;
}

} // namespace Git
