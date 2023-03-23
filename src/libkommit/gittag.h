/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libkommit_export.h"
#include <QString>

namespace Git
{

class LIBKOMMIT_EXPORT Tag
{
public:
    Tag();

    Q_REQUIRED_RESULT const QString &name() const;
    void setName(const QString &newName);

    Q_REQUIRED_RESULT const QString &message() const;
    void setMessage(const QString &newMessage);

    Q_REQUIRED_RESULT const QString &taggerEmail() const;
    void setTaggerEmail(const QString &newTaggerEmail);

    Q_REQUIRED_RESULT const QString &taggerName() const;
    void setTaggerName(const QString &newTaggerName);

    Q_REQUIRED_RESULT const QString &commiterName() const;
    void setCommiterName(const QString &newCommiterName);

    Q_REQUIRED_RESULT const QString &commiterEmail() const;
    void setCommiterEmail(const QString &newCommiterEmail);

private:
    QString mName;
    QString mMessage;

    QString mTaggerName;
    QString mTaggerEmail;
    QString mCommiterName;
    QString mCommiterEmail;
};

} // namespace Git
