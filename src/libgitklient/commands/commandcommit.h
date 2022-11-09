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
    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT const QString &message() const;
    void setMessage(const QString &newMessage);

    Q_REQUIRED_RESULT bool amend() const;
    void setAmend(bool newAmend);

    Q_REQUIRED_RESULT OptionalBool includeStatus() const;
    void setIncludeStatus(OptionalBool newIncludeStatus);

    bool parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;

private:
    QString mMessage;
    bool mAmend{false};
    OptionalBool mIncludeStatus{false};
};

} // namespace Git
