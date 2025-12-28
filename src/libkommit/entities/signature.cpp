/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signature.h"

#include <git2/signature.h>

#include <QTimeZone>

#include <Kommit/Error>
#include <Kommit/Repository>
namespace Git
{

class SignaturePrivate
{
public:
    SignaturePrivate();
    explicit SignaturePrivate(Repository *repo);
    SignaturePrivate(const QString &name, const QString &email);
    explicit SignaturePrivate(git_signature *signature);
    explicit SignaturePrivate(const git_signature *signature);
    ~SignaturePrivate();
    git_signature *signature{nullptr};

private:
    bool own;
};

SignaturePrivate::SignaturePrivate()
    : own{false}
{
}

SignaturePrivate::SignaturePrivate(Repository *repo)
    : own{true}
{
    git_signature_default(&signature, repo->repoPtr());
}

SignaturePrivate::SignaturePrivate(const QString &name, const QString &email)
    : own{true}
{
    git_signature_now(&signature, name.toUtf8().data(), email.toUtf8().data());
}

SignaturePrivate::SignaturePrivate(git_signature *signature)
    : signature{signature}
    , own{true}
{
}

SignaturePrivate::SignaturePrivate(const git_signature *signature)
    : signature{const_cast<git_signature *>(signature)}
    , own{false}
{
}

SignaturePrivate::~SignaturePrivate()
{
    if (own)
        git_signature_free(const_cast<git_signature *>(signature));
}

Signature::Signature()
    : d{new SignaturePrivate}
{
}

Signature::Signature(Repository *repo)
    : d{new SignaturePrivate{repo}}
{
}

Signature::Signature(const QString &name, const QString &email)
    : d{new SignaturePrivate{name, email}}
{
}

Signature::Signature(git_signature *signature)
    : d{new SignaturePrivate{signature}}
{
}

Signature::Signature(const git_signature *signature)
    : d{new SignaturePrivate{signature}}
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

void Signature::setName(const QString &name)
{
    QByteArray utf8 = name.toUtf8();
    delete[] d->signature->name;
    d->signature->name = new char[utf8.size() + 1];
    qstrcpy(d->signature->name, utf8.constData());
}

QString Signature::email() const
{
    return QString{d->signature->email};
}

void Signature::setEmail(const QString &email)
{
    QByteArray utf8 = email.toUtf8();
    delete[] d->signature->email;
    d->signature->email = new char[utf8.size() + 1];
    qstrcpy(d->signature->email, utf8.constData());
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

void Signature::setTime(const QDateTime &time)
{
    d->signature->when.time = time.toSecsSinceEpoch();

    int offsetMinutes = time.offsetFromUtc() / 60;

    if (offsetMinutes >= 0) {
        d->signature->when.sign = '+';
        d->signature->when.offset = offsetMinutes;
    } else {
        d->signature->when.sign = '-';
        d->signature->when.offset = -offsetMinutes;
    }
}
}
