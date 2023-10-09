/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signature.h"

#include <git2/signature.h>

namespace Git
{

Signature::Signature(git_signature *signature)
    : mSignature{signature}
{
    mName = signature->name;
    mEmail = signature->email;
    mTime = QDateTime::fromMSecsSinceEpoch(signature->when.time);
}

Signature::Signature(const git_signature *signature)
{
    mName = signature->name;
    mEmail = signature->email;
    mTime = QDateTime::fromMSecsSinceEpoch(signature->when.time);
}

Signature::~Signature()
{
    git_signature_free(mSignature);
}

QString Signature::name() const
{
    return mName;
}

QString Signature::email() const
{
    return mEmail;
}

QDateTime Signature::time() const
{
    return mTime;
}
}
