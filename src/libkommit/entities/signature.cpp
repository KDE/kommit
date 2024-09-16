/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signature.h"

#include <git2/signature.h>

#include <QTimeZone>

namespace Git
{

Signature::Signature(git_signature *signature)
    : mSignature{signature}
{
    mName = signature->name;
    mEmail = signature->email;
    QTimeZone timeZone{signature->when.offset};
    mTime = QDateTime::fromSecsSinceEpoch(signature->when.time, timeZone);
}

Signature::Signature(const git_signature *signature)
{
    mName = signature->name;
    mEmail = signature->email;
    QTimeZone timeZone{signature->when.offset};
    mTime = QDateTime::fromSecsSinceEpoch(signature->when.time, timeZone);
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

git_signature *Signature::signaturePtr() const
{
    return mSignature;
}
}
