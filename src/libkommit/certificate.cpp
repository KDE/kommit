#include "certificate.h"

#ifdef LIBKOMMIT_USE_OPENSSL
#include <openssl/pem.h>
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

namespace Impl
{

void get(const git_cert_x509 *x509_cert, Certificate::X509Data *data)
{
    if (!x509_cert || !x509_cert->data || x509_cert->len == 0 || !data) {
        printf("Invalid X.509 certificate or data pointer.\n");
        return;
    }

    // Create a BIO for reading the DER data
    BIO *bio = BIO_new_mem_buf(x509_cert->data, (int)x509_cert->len);
    if (!bio) {
        printf("Failed to create BIO.\n");
        return;
    }

    // Parse the DER-encoded certificate to get an X509 structure
    X509 *cert = d2i_X509_bio(bio, NULL);
    if (!cert) {
        printf("Failed to parse X.509 certificate.\n");
        BIO_free(bio);
        return;
    }

    // Extract the subject name
    char *subject_name = X509_NAME_oneline(X509_get_subject_name(cert), NULL, 0);
    data->subject = subject_name ? QString::fromUtf8(subject_name) : "Unknown";
    OPENSSL_free(subject_name);

    // Extract the issuer name
    char *issuer_name = X509_NAME_oneline(X509_get_issuer_name(cert), NULL, 0);
    data->issuer = issuer_name ? QString::fromUtf8(issuer_name) : "Unknown";
    OPENSSL_free(issuer_name);

    // Extract the serial number
    ASN1_INTEGER *serial = X509_get_serialNumber(cert);
    if (serial) {
        BIGNUM *bn = ASN1_INTEGER_to_BN(serial, NULL);
        if (bn) {
            char *serial_str = BN_bn2hex(bn);
            data->serialNumber = serial_str ? QString::fromUtf8(serial_str) : "Unknown";
            OPENSSL_free(serial_str);
            BN_free(bn);
        } else {
            data->serialNumber = "Unknown";
        }
    }

    // Extract the validity period
    BIO *time_bio = BIO_new(BIO_s_mem());
    ASN1_TIME_print(time_bio, X509_get0_notBefore(cert));
    char valid_from[256];
    BIO_read(time_bio, valid_from, sizeof(valid_from) - 1);
    data->validFrom = QString::fromUtf8(valid_from);

    BIO_reset(time_bio);
    ASN1_TIME_print(time_bio, X509_get0_notAfter(cert));
    char valid_until[256];
    BIO_read(time_bio, valid_until, sizeof(valid_until) - 1);
    data->validUntil = QString::fromUtf8(valid_until);
    BIO_free(time_bio);

    // Extract the public key information
    EVP_PKEY *pkey = X509_get_pubkey(cert);
    if (pkey) {
        int key_type = EVP_PKEY_base_id(pkey);
        int key_bits = EVP_PKEY_bits(pkey);
        data->publicKeyBits = QString::number(key_bits);

        switch (key_type) {
        case EVP_PKEY_RSA:
            data->publicKeyType = "RSA";
            break;
        case EVP_PKEY_DSA:
            data->publicKeyType = "DSA";
            break;
        case EVP_PKEY_EC:
            data->publicKeyType = "EC";
            break;
        default:
            data->publicKeyType = "Unknown";
            break;
        }
        EVP_PKEY_free(pkey);
    }

    // Extract the signature algorithm
    const X509_ALGOR *sig_alg = NULL;
    X509_get0_signature(NULL, &sig_alg, cert);
    if (sig_alg) {
        int sig_nid = OBJ_obj2nid(sig_alg->algorithm);
        const char *sig_algo = sig_nid != NID_undef ? OBJ_nid2ln(sig_nid) : "Unknown";
        data->signatureAlgorithm = QString::fromUtf8(sig_algo);
    } else {
        data->signatureAlgorithm = "Unknown";
    }

    // Clean up
    X509_free(cert);
    BIO_free(bio);
}

} // namespace Impl

class CertificatePrivate
{
    Certificate *q_ptr;
    Q_DECLARE_PUBLIC(Certificate)

public:
    explicit CertificatePrivate(Certificate *parent);

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
            // X509 *x509 = (X509 *)(cert_x509->data);

            Impl::get(cert_x509, &d->x509);
            // if (x509) {

            // d->readX509(x509);
            // GET_X509_VAR(X509_get_subject_name, subject);
            // GET_X509_VAR(X509_get_issuer_name, issuer);
            // GET_X509_VAR(X509_get_serialNumber, serialNumber);
            // }
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
