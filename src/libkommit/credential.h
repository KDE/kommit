/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>
#include <QSharedPointer>

#include <git2/credential.h>

namespace Git
{

class CredentialPrivate;
class LIBKOMMIT_EXPORT Credential
{
    Q_GADGET
public:
    enum class AllowedType {
        UserpassPlaintext = GIT_CREDENTIAL_USERPASS_PLAINTEXT,
        SshKey = GIT_CREDENTIAL_SSH_KEY,
        SshCustom = GIT_CREDENTIAL_SSH_CUSTOM,
        Defaut = GIT_CREDENTIAL_DEFAULT,
        SshInteractive = GIT_CREDENTIAL_SSH_INTERACTIVE,
        Username = GIT_CREDENTIAL_USERNAME,
        SshMemory = GIT_CREDENTIAL_SSH_MEMORY,
    };
    Q_DECLARE_FLAGS(AllowedTypes, AllowedType)
    Q_FLAG(AllowedTypes)

    Credential();
    Credential(const Credential &other);
    Credential &operator=(const Credential &other);
    bool operator==(const Credential &other) const;
    bool operator!=(const Credential &other) const;

    [[nodiscard]] QString username() const;
    void setUsername(const QString &username);

    [[nodiscard]] QString password() const;
    void setPassword(const QString &password);

    [[nodiscard]] AllowedTypes allowedTypes() const;
    void setAllowedTypes(const AllowedTypes &allowedTypes);

private:
    QSharedPointer<CredentialPrivate> d;
};

}
