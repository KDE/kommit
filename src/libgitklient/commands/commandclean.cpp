#include "commandclean.h"

namespace Git
{

CommandClean::CommandClean() = default;

QStringList CommandClean::generateArgs() const
{
    QStringList cmd{QStringLiteral("clean")};

    switch (type) {
    case AllUntrackedFiles:
        cmd << QStringLiteral("-fx");
        break;
    case NonIgnoredUntrackedFiles:
        cmd << QStringLiteral("-f");
        break;
    case IgnoredFiles:
        cmd << QStringLiteral("-fX");
        break;
    }

    if (mDryRun)
        cmd << QStringLiteral("--dry-run");

    if (mRemoveUntrackedDirectories)
        cmd << QStringLiteral("-d");

    return cmd;
}

bool CommandClean::dryRun() const
{
    return mDryRun;
}

void CommandClean::setDryRun(bool newDryRun)
{
    mDryRun = newDryRun;
}

CommandClean::CleanupType CommandClean::getType() const
{
    return type;
}

void CommandClean::setType(CleanupType newType)
{
    type = newType;
}

bool CommandClean::removeUntrackedDirectories() const
{
    return mRemoveUntrackedDirectories;
}

void CommandClean::setRemoveUntrackedDirectories(bool newRemoveUntrackedDirectories)
{
    mRemoveUntrackedDirectories = newRemoveUntrackedDirectories;
}

}
