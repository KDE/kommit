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

class SignaturePrivate;
class LIBKOMMIT_EXPORT Signature
{
public:
    explicit Signature(git_signature *signature = nullptr);
    explicit Signature(const git_signature *signature);
    Signature(const Signature &other);
    Signature &operator=(const Signature &other);
    bool operator==(const Signature &other) const;
    bool operator!=(const Signature &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_signature *data() const;
    [[nodiscard]] const git_signature *constData() const;

    [[nodiscard]] QString name() const;
    [[nodiscard]] QString email() const;
    [[nodiscard]] QDateTime time() const;

private:
    QSharedPointer<SignaturePrivate> d;
};
}
