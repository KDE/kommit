/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandchangedfiles.h"
#include "git/gitmanager.h"

namespace Git
{

CommandChangedFiles::CommandChangedFiles(Manager *git)
    : AbstractCommand(git)
{
}

const QList<FileStatus> &CommandChangedFiles::files() const
{
    return mFiles;
}

bool CommandChangedFiles::ignored() const
{
    return mIgnored;
}

void CommandChangedFiles::setIgnored(bool newIgnored)
{
    mIgnored = newIgnored;
}

bool CommandChangedFiles::untracked() const
{
    return mUntracked;
}

void CommandChangedFiles::setUntracked(bool newUntracked)
{
    mUntracked = newUntracked;
}

bool CommandChangedFiles::ignoreSubmodules() const
{
    return mIgnoreSubmodules;
}

void CommandChangedFiles::setIgnoreSubmodules(bool newIgnoreSubmodules)
{
    mIgnoreSubmodules = newIgnoreSubmodules;
}

QStringList CommandChangedFiles::generateArgs() const
{
    QStringList args{QStringLiteral("status"), QStringLiteral("--short"), QStringLiteral("--porcelain")};
    if (mUntracked)
        args.append(QStringLiteral("--untracked-files=all"));

    if (mIgnoreSubmodules)
        args.append(QStringLiteral("--ignore-submodules"));

    if (mIgnored)
        args.append(QStringLiteral("--ignored"));

    return args;
}

void CommandChangedFiles::parseOutput(const QByteArray &output, const QByteArray &errorOutput)
{
    Q_UNUSED(errorOutput)
    const auto buffer = QString(output).split(QLatin1Char('\n'));

    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;
        FileStatus fs;
        fs.parseStatusLine(item);
        fs.setFullPath(mGit->path() + fs.name());
        mFiles.append(fs);
    }
}

} // namespace Git
