/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/types.h>

#include <QDateTime>
#include <QSharedPointer>
#include <QString>

namespace Git
{

class Repository;
class SignaturePrivate;
class LIBKOMMIT_EXPORT Signature
{
public:
    Signature();
    explicit Signature(Repository *repo);
    Signature(const QString &name, const QString &email);
    explicit Signature(git_signature *signature);
    explicit Signature(const git_signature *signature);
    Signature(const Signature &other) = default;
    Signature &operator=(const Signature &other);
    bool operator==(const Signature &other) const;
    bool operator!=(const Signature &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_signature *data() const;
    [[nodiscard]] const git_signature *constData() const;

    [[nodiscard]] QString name() const;
    void setName(const QString &name);
    [[nodiscard]] QString email() const;
    void setEmail(const QString &email);
    [[nodiscard]] QDateTime time() const;
    void setTime(const QDateTime &time);

private:
    QSharedPointer<SignaturePrivate> d;
};
}
