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
    enum CleanupType {
        AllUntrackedFiles,
        NonIgnoredUntrackedFiles,
        IgnoredFiles,
    };
    CommandClean();

    [[nodiscard]] QStringList generateArgs() const override;

    [[nodiscard]] bool dryRun() const;
    void setDryRun(bool newDryRun);
    [[nodiscard]] CleanupType getType() const;
    void setType(CleanupType newType);
    [[nodiscard]] bool removeUntrackedDirectories() const;
    void setRemoveUntrackedDirectories(bool newRemoveUntrackedDirectories);

private:
    bool mDryRun{false};
    CleanupType type{NonIgnoredUntrackedFiles};
    bool mRemoveUntrackedDirectories{false};
};

}
