/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signature.h"

#include <git2/signature.h>

#include <QTimeZone>

namespace Git
{

class SignaturePrivate
{
public:
    SignaturePrivate();
    explicit SignaturePrivate(git_signature *signature);
    explicit SignaturePrivate(const git_signature *signature);
    ~SignaturePrivate();
    const git_signature *signature;

private:
    bool own;
};

SignaturePrivate::SignaturePrivate()
    : signature{nullptr}
    , own{false}
{
}

SignaturePrivate::SignaturePrivate(git_signature *signature)
    : signature{signature}
    , own{true}
{
}

SignaturePrivate::SignaturePrivate(const git_signature *signature)
    : signature{signature}
    , own{false}
{
}

SignaturePrivate::~SignaturePrivate()
{
    if (own)
        git_signature_free(const_cast<git_signature *>(signature));
}

Signature::Signature()
    : d{new SignaturePrivate{}}
{
    // git_signature_now(&d->signature, "", "");
}

Signature::Signature(git_signature *signature)
    : d{new SignaturePrivate{signature}}
{
}

Signature::Signature(const git_signature *signature)
    : d{new SignaturePrivate{signature}}
{
}

Signature::Signature(const Signature &other)
    : d{other.d}
{
}

Signature &Signature::operator=(const Signature &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Signature::operator==(const Signature &other) const
{
    return d->signature == other.d->signature;
}

bool Signature::operator!=(const Signature &other) const
{
    return !(*this == other);
}

bool Signature::isNull() const
{
    return !d->signature;
}

git_signature *Signature::data() const
{
    return const_cast<git_signature *>(d->signature);
}

const git_signature *Signature::constData() const
{
    return d->signature;
}

QString Signature::name() const
{
    return QString{d->signature->name};
}

QString Signature::email() const
{
    return QString{d->signature->email};
}

QDateTime Signature::time() const
{
    if (d->signature->when.sign == '+') {
        QTimeZone timeZone{d->signature->when.offset * 60};
        return QDateTime::fromSecsSinceEpoch(d->signature->when.time, timeZone);
    } else {
        QTimeZone timeZone{-d->signature->when.offset * 60};
        return QDateTime::fromSecsSinceEpoch(d->signature->when.time, timeZone);
    }
}

}
