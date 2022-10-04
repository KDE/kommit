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
    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT OptionalBool commit() const;
    void setCommit(OptionalBool newCommit);

    Q_REQUIRED_RESULT OptionalBool allowUnrelatedHistories() const;
    void setAllowUnrelatedHistories(OptionalBool newAllowUnrelatedHistories);

    FastForwardType ff() const;
    void setFf(FastForwardType newFf);

    Q_REQUIRED_RESULT bool squash() const;
    void setSquash(bool newSquash);

    Q_REQUIRED_RESULT const QString &fromBranch() const;
    void setFromBranch(const QString &newFromBranch);

    Q_REQUIRED_RESULT const QString &strategy() const;
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
