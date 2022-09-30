/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"
#include "libgitklient_export.h"

namespace Git
{

class LIBGITKLIENT_EXPORT CommandMerge : public AbstractCommand
{
    Q_OBJECT

public:
    explicit CommandMerge(Manager *git);
    QStringList generateArgs() const override;

    OptionalBool commit() const;
    void setCommit(OptionalBool newCommit);

    OptionalBool allowUnrelatedHistories() const;
    void setAllowUnrelatedHistories(OptionalBool newAllowUnrelatedHistories);

    FastForwardType ff() const;
    void setFf(FastForwardType newFf);

    bool squash() const;
    void setSquash(bool newSquash);

    const QString &fromBranch() const;
    void setFromBranch(const QString &newFromBranch);

    const QString &strategy() const;
    void setStrategy(const QString &newStrategy);

private:
    OptionalBool mCommit{OptionalBool::Unset};
    OptionalBool mAllowUnrelatedHistories{OptionalBool::Unset};
    FastForwardType mFf{FastForwardType::Unset};
    bool mSquash{false};
    QString mFromBranch;
    QString mStrategy;
};

}
