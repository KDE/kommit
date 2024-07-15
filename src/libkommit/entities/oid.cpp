/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "oid.h"

#include <git2/oid.h>

#define GIT_OID_SHA1_SIZE 20
#define GIT_OID_SHA1_HEXSIZE (GIT_OID_SHA1_SIZE * 2)

namespace Git
{

Oid::Oid(const git_oid *oid)
    : mOidPtr{oid}
{
}

QString Oid::toString() const
{
    char str[GIT_OID_SHA1_HEXSIZE + 1];
    if (git_oid_fmt(str, mOidPtr))
        return {};
    str[GIT_OID_SHA1_HEXSIZE] = '\0';

    return QString{str};
}

bool Oid::isNull() const
{
    return git_oid_is_zero(mOidPtr);
}

const git_oid *Oid::oidPtr() const
{
    return mOidPtr;
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
