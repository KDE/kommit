/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "abstractcommand.h"
#include "libkommit_export.h"

namespace Git
{

class LIBKOMMIT_EXPORT CommandSwitchBranch : public AbstractCommand
{
    Q_OBJECT

public:
    explicit CommandSwitchBranch(Manager *git);
    void parseOutputSection(const QByteArray &output, const QByteArray &errorOutput) override;

    enum Mode {
        Unknown,
        NewBranch,
        ExistingBranch,
        Tag,
        RemoteBranch,
    };

    [[nodiscard]] const QString &target() const;
    void setTarget(const QString &newTarget);

    [[nodiscard]] Mode mode() const;
    void setMode(Mode newMode);

    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] bool force() const;
    void setForce(bool newForce);

    [[nodiscard]] QString remoteBranch() const;
    void setRemoteBranch(const QString &newRemoteBranch);

private:
    bool mForce{false};
    Mode mMode{Unknown};
    QString mTarget;
    QString mRemoteBranch;
};

} // namespace Git
