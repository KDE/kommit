/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractcommand.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT CommandPush : public AbstractCommand
{
public:
    CommandPush();
    QStringList generateArgs() const override;

    Q_REQUIRED_RESULT bool force() const;
    void setForce(bool newForce);

    Q_REQUIRED_RESULT const QString &remote() const;
    void setRemote(const QString &newRemote);

    Q_REQUIRED_RESULT const QString &localBranch() const;
    void setLocalBranch(const QString &newBranch);

    Q_REQUIRED_RESULT const QString &remoteBranch() const;
    void setRemoteBranch(const QString &newRemoteBranch);

private:
    QString mRemote;
    QString mLocalBranch;
    QString mRemoteBranch;
    bool mForce{false};
};

} // namespace Git
