/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "abstractcommand.h"

namespace Git
{

class LIBKOMMIT_EXPORT CommandClean : public AbstractCommand
{
    Q_OBJECT

public:
    enum CleanupType { AllUntrackedFiles, NonIgnoredUntrackedFiles, IgnoredFiles };
    CommandClean();

    Q_REQUIRED_RESULT QStringList generateArgs() const override;

    Q_REQUIRED_RESULT bool dryRun() const;
    void setDryRun(bool newDryRun);
    Q_REQUIRED_RESULT CleanupType getType() const;
    void setType(CleanupType newType);
    Q_REQUIRED_RESULT bool removeUntrackedDirectories() const;
    void setRemoveUntrackedDirectories(bool newRemoveUntrackedDirectories);

private:
    bool mDryRun{false};
    CleanupType type{NonIgnoredUntrackedFiles};
    bool mRemoveUntrackedDirectories{false};
};

}
