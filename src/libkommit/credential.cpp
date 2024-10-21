/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "credential.h"

namespace Git
{

class CredentialPrivate
{
public:
    QString username;
    QString password;
    Credential::AllowedTypes allowedTypes;
};

Credential::Credential()
    : d{new CredentialPrivate}
{
}

Credential::Credential(const Credential &other)
    : d{other.d}
{
}

Credential &Credential::operator=(const Credential &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Credential::operator==(const Credential &other) const
{
    return d->username == other.d->username && d->password == other.d->password && d->allowedTypes == other.d->allowedTypes;
}

bool Credential::operator!=(const Credential &other) const
{
    return !(*this == other);
}

QString Credential::username() const
{
    return d->username;
}

void Credential::setUsername(const QString &username)
{
    d->username = username;
}

QString Credential::password() const
{
    return d->password;
}

void Credential::setPassword(const QString &password)
{
    d->password = password;
}

Credential::AllowedTypes Credential::allowedTypes() const
{
    return d->allowedTypes;
}

void Credential::setAllowedTypes(const AllowedTypes &allowedTypes)
{
    d->allowedTypes = allowedTypes;
}
}

#include "moc_credential.cpp"
