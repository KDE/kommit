/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signature.h"

namespace Git
{

Signature::Signature(git_signature *signature)
{
    mName = signature->name;
    mEmail = signature->email;
}

QString Signature::name() const
{
    return mName;
}

QString Signature::email() const
{
    return mEmail;
}

}
