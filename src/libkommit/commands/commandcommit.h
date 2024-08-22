/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libkommit_export.h"
namespace Git
{

class LIBKOMMIT_EXPORT CommandCommit : public AbstractCommand
{
public:
    CommandCommit();
    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] const QString &message() const;
    void setMessage(const QString &newMessage);

    [[nodiscard]] bool amend() const;
    void setAmend(bool newAmend);

    [[nodiscard]] OptionalBool includeStatus() const;
    void setIncludeStatus(OptionalBool newIncludeStatus);

    [[nodiscard]] bool parseOutput(const QByteArray &output, const QByteArray &errorOutput) override;

private:
    QString mMessage;
    bool mAmend{false};
    OptionalBool mIncludeStatus{false};
};

} // namespace Git
