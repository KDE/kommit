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

    Q_REQUIRED_RESULT const QString &name() const;
    void setName(const QString &newName);

    Q_REQUIRED_RESULT const QString &message() const;
    void setMessage(const QString &newMessage);

    Q_REQUIRED_RESULT const QString &taggerEmail() const;
    void setTaggerEmail(const QString &newTaggerEmail);

private:
    QString mName;
    QString mMessage;
    QString mTaggerEmail;
};

} // namespace Git
