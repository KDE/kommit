/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signature.h"

namespace Git
{

Signature::Signature()
{
}

Signature::Signature(const git_signature *signature)
{
    setSignature(signature);
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

void Signature::setSignature(const git_signature *signature)
{
    mSignature = signature;
    mName = signature->name;
    mEmail = signature->email;
    mTime = QDateTime::fromMSecsSinceEpoch(signature->when.offset);
}
}
