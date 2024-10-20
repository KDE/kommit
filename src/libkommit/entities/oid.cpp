/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "oid.h"

namespace Git
{

Oid::Oid() noexcept
    : d(GIT_OID_SHA1_SIZE, 0)
{
}

Oid::Oid(const git_oid *oid)
    : Oid{}
{
    if (oid)
        git_oid_cpy(data(), oid);
}

Oid::Oid(const git_oid oid)
    : Oid{}
{
    git_oid_cpy(data(), &oid);
}

Oid::Oid(const Oid &other)
    : d{other.d}
{
}

Oid &Oid::operator=(const Oid &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Oid::operator==(const Oid &other)
{
    return d == other.d;
}

bool Oid::operator!=(const Oid &other)
{
    return !(*this == other);
}

QString Oid::toString() const
{
    QByteArray ba(GIT_OID_SHA1_HEXSIZE, Qt::Uninitialized);
    git_oid_fmt(ba.data(), constData());
    return QString{ba};
}

bool Oid::isNull() const
{
    return d.isEmpty();
}

git_oid *Oid::data() const
{
    return reinterpret_cast<git_oid *>(const_cast<Oid *>(this)->d.data());
}

const git_oid *Oid::constData() const
{
    return reinterpret_cast<const git_oid *>(d.constData());
}
}

bool operator==(const Git::Oid &oid, const QString &hash)
{
    return oid.toString() == hash;
}

bool operator!=(const Git::Oid &oid, const QString &hash)
{
    return oid.toString() != hash;
}
