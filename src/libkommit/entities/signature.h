/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <git2/types.h>

#include <QDateTime>
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Signature
{
public:
    explicit Signature(git_signature *signature);
    explicit Signature(const git_signature *signature);
    ~Signature();

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString email() const;
    Q_REQUIRED_RESULT QDateTime time() const;

private:
    git_signature *mSignature{nullptr};
    QString mName;
    QString mEmail;
    QDateTime mTime;
};
}
