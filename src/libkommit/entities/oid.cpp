/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "oid.h"

#define GIT_OID_SHA1_SIZE 20
#define GIT_OID_SHA1_HEXSIZE (GIT_OID_SHA1_SIZE * 2)

namespace Git
{

Oid::Oid()
{
}

Oid::Oid(const git_oid *oid)
{
    if (oid)
        git_oid_cpy(&mOid, oid);
}

Oid::Oid(const git_oid oid)
{
    // mOid member of class is git_oid *{nullptr}
    //  fill mOid by oid
    // mOid = new git_oid;
    // auto o = const_cast<git_oid *>(mOid);
    // git_oid_cpy(o, &oid);
    git_oid_cpy(&mOid, &oid);
}

Oid::~Oid()
{
}

QString Oid::toString() const
{
    // if (!mOid)
    // return {};

    char str[GIT_OID_SHA1_HEXSIZE + 1];
    if (git_oid_fmt(str, &mOid))
        return {};
    str[GIT_OID_SHA1_HEXSIZE] = '\0';

    return QString{str};
}

bool Oid::isNull() const
{
    // if (!mOid)
    // return true;
    return git_oid_is_zero(&mOid);
}

void Oid::copyTo(git_oid *oid)
{
    git_oid_cpy(oid, &mOid);
}

const git_oid *Oid::oidPtr() const
{
    return &mOid;
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
