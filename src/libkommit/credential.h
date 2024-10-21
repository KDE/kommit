/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>
#include <QSharedPointer>

namespace Git
{

class CredentialPrivate;
class LIBKOMMIT_EXPORT Credential
{
    Q_GADGET
public:
    enum class AllowedType {
        UserpassPlaintext,
        SshKey,
        SshCustom,
        Defaut,
        SshInteractive,
        Username,
        SshMemory,
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
