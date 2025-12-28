/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "manager.h"

#include <QProcess>
#include <gitglobal.h>

namespace Git
{

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
    p.setProgram(QStringLiteral("git"));
    p.setArguments({QStringLiteral("rev-parse"), QStringLiteral("--show-toplevel")});
    p.setWorkingDirectory(newPath);
    p.start();
    p.waitForFinished();
    auto ret = p.readAllStandardOutput() + p.readAllStandardError();

    if (p.exitStatus() == QProcess::CrashExit || ret == QLatin1String() || ret.contains("fatal")) {
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
    // const auto buffer = Git::readAllNonEmptyOutput(mPath,
    //                                                {QStringLiteral("status"),
    //                                                 QStringLiteral("--untracked-files=all"),
    //                                                 QStringLiteral("--ignored"),
    //                                                 QStringLiteral("--short"),
    //                                                 QStringLiteral("--ignore-submodules"),
    //                                                 QStringLiteral("--porcelain")},
    //                                                false);

    // QList<FileStatus> files;
    // QList<FileStatus> unknownFiles;
    // // TODO: read untrackeds
    // for (const auto &item : buffer) {
    //     if (item.trimmed().isEmpty())
    //         continue;

    //     FileStatus fs;
    //     fs.parseStatusLine(item);
    //     fs.setFullPath(mPath + QLatin1Char('/') + fs.name());

    //     if (item.startsWith(QStringLiteral("??")))
    //         unknownFiles << fs;
    //     else
    //         files.append(fs);
    // }
    // for (auto &f : unknownFiles) {
    //     const auto n = files.indexOf(f);
    //     if (n == -1) {
    //         f.setStatus(FileStatus::Untracked);
    //         files.append(f);
    //     }
    // }
    // return files;
    // TODO: fix this
    return {};
}

void MiniManager::repoFilesStatus(const std::function<bool(const FileStatus &)> &cb) const
{
    // const auto buffer = Git::readAllNonEmptyOutput(mPath,
    //                                                {QStringLiteral("status"),
    //                                                 QStringLiteral("--untracked-files=all"),
    //                                                 QStringLiteral("--ignored"),
    //                                                 QStringLiteral("--short"),
    //                                                 QStringLiteral("--ignore-submodules"),
    //                                                 QStringLiteral("--porcelain")},
    //                                                false);

    // QList<FileStatus> unknownFiles;
    // // TODO: read untrackeds
    // for (const auto &item : buffer) {
    //     if (item.trimmed().isEmpty())
    //         continue;

    //     FileStatus fs;
    //     fs.parseStatusLine(item);
    //     fs.setFullPath(mPath + QLatin1Char('/') + fs.name());

    //     if (item.startsWith(QStringLiteral("??")))
    //         unknownFiles << fs;
    //     else if (!cb(fs))
    //         return;
    //     // files.append(fs);
    // }
    // //    for (auto &f : unknownFiles) {
    // //        const auto n = files.indexOf(f);
    // //        if (n == -1) {
    // //            f.setStatus(FileStatus::Untracked);
    // //            files.append(f);
    // //        }
    // //    }
    // TODO: fix this
}
}
