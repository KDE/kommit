/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include <QString>

namespace Git
{

class LIBGITKLIENT_EXPORT Tag
{
public:
    Tag();

    const QString &name() const;
    void setName(const QString &newName);

    const QString &message() const;
    void setMessage(const QString &newMessage);

    const QString &taggerEmail() const;
    void setTaggerEmail(const QString &newTaggerEmail);

private:
    QString mName;
    QString mMessage;
    QString mTaggerEmail;
};

} // namespace Git
