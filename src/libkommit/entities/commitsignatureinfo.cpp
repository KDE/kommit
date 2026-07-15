/*
SPDX-FileCopyrightText: 2026 Li Bohai <lbhlbhlbh2002@icloud.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitsignatureinfo.h"

#include <QCoreApplication>

namespace Git
{

class CommitSignatureInfo::Private
{
public:
    CommitSignatureInfo::VerificationStatus status{CommitSignatureInfo::None};
    CommitSignatureInfo::SignatureType type{CommitSignatureInfo::NoSignature};
    QString signer;
    QString email;
    QString keyId;
    QString fingerprint;
    QString trustLevel;
    QString rawOutput;
};

CommitSignatureInfo::CommitSignatureInfo()
    : d{new Private}
{
}

CommitSignatureInfo::~CommitSignatureInfo() = default;

CommitSignatureInfo::CommitSignatureInfo(const CommitSignatureInfo &other)
    : d{other.d}
{
}

CommitSignatureInfo &CommitSignatureInfo::operator=(const CommitSignatureInfo &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

bool CommitSignatureInfo::isNull() const
{
    return d->status == None;
}

CommitSignatureInfo::VerificationStatus CommitSignatureInfo::status() const
{
    return d->status;
}

void CommitSignatureInfo::setStatus(VerificationStatus status)
{
    d->status = status;
}

CommitSignatureInfo::SignatureType CommitSignatureInfo::type() const
{
    return d->type;
}

void CommitSignatureInfo::setType(SignatureType type)
{
    d->type = type;
}

const QString &CommitSignatureInfo::signer() const
{
    return d->signer;
}

void CommitSignatureInfo::setSigner(const QString &signer)
{
    d->signer = signer;
}

const QString &CommitSignatureInfo::email() const
{
    return d->email;
}

void CommitSignatureInfo::setEmail(const QString &email)
{
    d->email = email;
}

const QString &CommitSignatureInfo::keyId() const
{
    return d->keyId;
}

void CommitSignatureInfo::setKeyId(const QString &keyId)
{
    d->keyId = keyId;
}

const QString &CommitSignatureInfo::fingerprint() const
{
    return d->fingerprint;
}

void CommitSignatureInfo::setFingerprint(const QString &fingerprint)
{
    d->fingerprint = fingerprint;
}

const QString &CommitSignatureInfo::trustLevel() const
{
    return d->trustLevel;
}

void CommitSignatureInfo::setTrustLevel(const QString &trustLevel)
{
    d->trustLevel = trustLevel;
}

const QString &CommitSignatureInfo::rawOutput() const
{
    return d->rawOutput;
}

void CommitSignatureInfo::setRawOutput(const QString &rawOutput)
{
    d->rawOutput = rawOutput;
}

QString CommitSignatureInfo::statusText() const
{
    switch (d->status) {
    case Good:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Good signature");
    case Bad:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Bad signature");
    case Unknown:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Signature cannot be verified");
    case ExpiredKey:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Signature made with expired key");
    case ExpiredSig:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Expired signature");
    case RevokedKey:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Signature made with revoked key");
    case MissingKey:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Signature cannot be verified (missing key)");
    case Error:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Signature verification error");
    case None:
    default:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "Not signed");
    }
}

QString CommitSignatureInfo::typeText() const
{
    switch (d->type) {
    case GpgSignature:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "GPG Signature");
    case SshSignature:
        return QCoreApplication::translate("Git::CommitSignatureInfo", "SSH Signature");
    case NoSignature:
    default:
        return {};
    }
}

QString CommitSignatureInfo::typeIconName() const
{
    switch (d->type) {
    case GpgSignature:
        return QStringLiteral("application-pgp-keys");
    case SshSignature:
        return QStringLiteral("dialog-password");
    case NoSignature:
    default:
        return QString();
    }
}

}
