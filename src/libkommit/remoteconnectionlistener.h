/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>

#include "libkommit_export.h"

namespace Git {

class Certificate;
class Credential;

class LIBKOMMIT_EXPORT RemoteConnectionListener
{
public:
    virtual void credentialRequeted(const QString &url, Credential *cred, bool *accept) = 0;
    virtual void certificateCheck(Certificate *cert, bool *accept) = 0;
};

}
