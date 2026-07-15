/*
SPDX-FileCopyrightText: 2026 Li Bohai <lbhlbhlbh2002@icloud.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QSharedPointer>
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT CommitSignatureInfo
{
public:
    enum VerificationStatus {
        None,
        Good,
        Bad,
        Unknown,
        ExpiredKey,
        ExpiredSig,
        RevokedKey,
        MissingKey,
        Error
    };

    enum SignatureType {
        NoSignature,
        GpgSignature,
        SshSignature
    };

    CommitSignatureInfo();
    ~CommitSignatureInfo();

    CommitSignatureInfo(const CommitSignatureInfo &other);
    CommitSignatureInfo &operator=(const CommitSignatureInfo &other);

    [[nodiscard]] bool isNull() const;

    [[nodiscard]] VerificationStatus status() const;
    void setStatus(VerificationStatus status);

    [[nodiscard]] SignatureType type() const;
    void setType(SignatureType type);

    [[nodiscard]] const QString &signer() const;
    void setSigner(const QString &signer);

    [[nodiscard]] const QString &email() const;
    void setEmail(const QString &email);

    [[nodiscard]] const QString &keyId() const;
    void setKeyId(const QString &keyId);

    [[nodiscard]] const QString &fingerprint() const;
    void setFingerprint(const QString &fingerprint);

    [[nodiscard]] const QString &trustLevel() const;
    void setTrustLevel(const QString &trustLevel);

    [[nodiscard]] const QString &rawOutput() const;
    void setRawOutput(const QString &rawOutput);

    [[nodiscard]] QString statusText() const;
    [[nodiscard]] QString typeText() const;
    [[nodiscard]] QString typeIconName() const;

private:
    class Private;
    QSharedPointer<Private> d;
};

}
