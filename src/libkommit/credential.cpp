/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "credential.h"

namespace Git
{

class CredentialPrivate
{
    Credential *q_ptr;
    Q_DECLARE_PUBLIC(Credential)

public:
    CredentialPrivate(Credential *parent);

    QString username;
    QString password;
    Credential::AllowedTypes allowedTypes;
};

Credential::Credential()
    : d_ptr{new CredentialPrivate{this}}
{
}

QString Credential::username() const
{
    Q_D(const Credential);
    return d->username;
}

void Credential::setUsername(const QString &username)
{
    Q_D(Credential);
    d->username = username;
}

QString Credential::password() const
{
    Q_D(const Credential);
    return d->password;
}

void Credential::setPassword(const QString &password)
{
    Q_D(Credential);
    d->password = password;
}

Credential::AllowedTypes Credential::allowedTypes() const
{
    Q_D(const Credential);
    return d->allowedTypes;
}

void Credential::setAllowedTypes(const AllowedTypes &allowedTypes)
{
    Q_D(Credential);
    d->allowedTypes = allowedTypes;
}

CredentialPrivate::CredentialPrivate(Credential *parent)
    : q_ptr{parent}
{
}

}

#include "moc_credential.cpp"
