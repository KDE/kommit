/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/types.h>

#include <QString>

namespace Git
{

class Signature
{
public:
    Signature(git_signature *signature);

    Q_REQUIRED_RESULT QString name() const;
    Q_REQUIRED_RESULT QString email() const;

private:
    QString mName;
    QString mEmail;
    QString QDateTime;
};

}
