/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libgitklient_export.h"
namespace Git
{

class LIBGITKLIENT_EXPORT CommandCommit : public AbstractCommand
{
public:
    CommandCommit();
    QStringList generateArgs() const override;

    const QString &message() const;
    void setMessage(const QString &newMessage);

    bool amend() const;
    void setAmend(bool newAmend);

    bool includeStatus() const;
    void setIncludeStatus(bool newIncludeStatus);

private:
    QString mMessage;
    bool mAmend{false};
    bool mIncludeStatus{false};
};

} // namespace Git
