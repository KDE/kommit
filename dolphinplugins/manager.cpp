/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "manager.h"

#include <gitglobal.h>
#include <QProcess>

namespace Git {

MiniManager::MiniManager(const QString &path)
{
    setPath(path);
}

const QString &MiniManager::path() const
{
    return mPath;
}

void MiniManager::setPath(const QString &newPath)
{
    if (mPath == newPath)
        return;

    QProcess p;
    p.setProgram("git");
    p.setArguments({"rev-parse", "--show-toplevel"});
    p.setWorkingDirectory(newPath);
    p.start();
    p.waitForFinished();
    auto ret = p.readAllStandardOutput() + p.readAllStandardError();

    if (ret.contains("fatal")) {
        mPath = QString();
        mIsValid = false;
    } else {
        mPath = ret.replace("\n", "");
        mIsValid = true;
    }
}

bool MiniManager::isValid() const
{
    return mIsValid;
}

QList<FileStatus> MiniManager::repoFilesStatus() const
{
    const auto buffer = Git::readAllNonEmptyOutput(mPath,
                                             {"status",
                                              "--untracked-files=all",
                                              "--ignored",
                                              "--short",
                                              "--ignore-submodules",
                                              "--porcelain"}, false);

    QList<FileStatus> files;
    //TODO: read untrackeds
    for (const auto &item : buffer) {
        if (!item.trimmed().size())
            continue;

        if (item.startsWith("??")) {

        }

        FileStatus fs;
        fs.parseStatusLine(item);
        fs.setFullPath(mPath + QLatin1Char('/') + fs.name());
        if (fs.status() != FileStatus::Untracked)// && !files.contains(fs))
            files.append(fs);

    }
    return files;
}

}
