#pragma once

#include <git2/cert.h>

#include <QScopedPointer>
#include <QString>

#include "libkommit_export.h"

namespace Git
{

class CertificatePrivate;
class LIBKOMMIT_EXPORT Certificate
{
public:
    enum class Type { None = GIT_CERT_NONE, X509 = GIT_CERT_X509, HostkeyLibssh2 = GIT_CERT_HOSTKEY_LIBSSH2, Strarray = GIT_CERT_STRARRAY };

    Certificate(git_cert *cert, bool valid, const QString &host);
    ~Certificate();

    enum class HostKeyHashType {
        Md5 = GIT_CERT_SSH_MD5,
        Sha1 = GIT_CERT_SSH_SHA1,
        Sha256 = GIT_CERT_SSH_SHA256,
        Raw = GIT_CERT_SSH_RAW,
    };

    struct X509Data {
        QString subject;
        QString issuer;
        QString serialNumber;
        QString validFrom;
        QString validUntil;
        QString publicKeyType;
        QString publicKeyBits;
        QString signatureAlgorithm;
    };
    struct HostKeyData {
        QByteArray hash;
        HostKeyHashType hashType;
    };

    [[nodiscard]] Type type() const;
    void setType(Type type);

    [[nodiscard]] bool isValid() const;
    void setIsValid(bool isValid);

    [[nodiscard]] QString host() const;
    void setHost(const QString &host);

    [[nodiscard]] const X509Data &x509() const;

    [[nodiscard]] const HostKeyData &hostKey() const;

private:
    QScopedPointer<CertificatePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Certificate)
};

}
