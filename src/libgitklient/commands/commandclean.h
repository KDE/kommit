/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef COMMANDCLEAN_H
#define COMMANDCLEAN_H

#include "abstractcommand.h"

namespace Git
{

class LIBGITKLIENT_EXPORT CommandClean : public AbstractCommand
{
    Q_OBJECT

public:
    enum CleanupType { AllUntrackedFiles, NonIgnoredUntrackedFiles, IgnoredFiles };
    CommandClean();

    QStringList generateArgs() const override;

    bool dryRun() const;
    void setDryRun(bool newDryRun);
    CleanupType getType() const;
    void setType(CleanupType newType);
    bool removeUntrackedDirectories() const;
    void setRemoveUntrackedDirectories(bool newRemoveUntrackedDirectories);

private:
    bool mDryRun{false};
    CleanupType type{NonIgnoredUntrackedFiles};
    bool mRemoveUntrackedDirectories{false};
};

}

#endif // COMMANDCLEAN_H
