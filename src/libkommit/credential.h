/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QObject>

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

    Q_REQUIRED_RESULT QString username() const;
    void setUsername(const QString &username);

    Q_REQUIRED_RESULT QString password() const;
    void setPassword(const QString &password);

    Q_REQUIRED_RESULT AllowedTypes allowedTypes() const;
    void setAllowedTypes(const AllowedTypes &allowedTypes);

private:
    CredentialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(Credential)
};

}
