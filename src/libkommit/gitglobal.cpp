/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "gitglobal.h"

#include "libkommit_debug.h"
#include <QIcon>
#include <QProcess>

namespace Git
{

void run(const QString &workingDir, const QStringList &args)
{
    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(workingDir);
    p.start();
    p.waitForFinished();
}
void run(const QString &workingDir, const AbstractCommand &cmd)
{
    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(cmd.generateArgs());
    p.setWorkingDirectory(workingDir);
    p.start();
    p.waitForFinished();
}

QString runGit(const QString &workingDir, const QStringList &args)
{
    //    qCDebug(KOMMITLIB_LOG).noquote() << "Running: git " << args.join(" ");

    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(workingDir);
    p.start();
    p.waitForFinished();
    auto out = QString::fromUtf8(p.readAllStandardOutput());
    //    auto err = p.readAllStandardError();

    return out; // + err;
}

QStringList readAllNonEmptyOutput(const QString &workingDir, const QStringList &cmd, bool trim)
{
    QStringList list;
    const auto out = runGit(workingDir, cmd).split(QLatin1Char('\n'));

    for (const auto &line : out) {
        QString b;

        if (trim)
            b = line.trimmed();
        else
            b = line;

        if (b.isEmpty())
            continue;

        list.append(b);
    }
    return list;
}

QIcon statusIcon(ChangeStatus status)
{
    switch (status) {
    case ChangeStatus::Added:
        return QIcon::fromTheme(QStringLiteral("git-status-added"));
    case ChangeStatus::Ignored:
        return QIcon::fromTheme(QStringLiteral("git-status-ignored"));
    case ChangeStatus::Modified:
        return QIcon::fromTheme(QStringLiteral("git-status-modified"));
    case ChangeStatus::Removed:
        return QIcon::fromTheme(QStringLiteral("git-status-removed"));
    case ChangeStatus::Renamed:
        return QIcon::fromTheme(QStringLiteral("git-status-renamed"));
        //    case ChangeStatus::Unknown:
    case ChangeStatus::Untracked:
        return QIcon::fromTheme(QStringLiteral("git-status-unknown"));
    case ChangeStatus::Copied:
    case ChangeStatus::UpdatedButInmerged:
    case ChangeStatus::Unmodified:
        return QIcon::fromTheme(QStringLiteral("git-status-update"));
    case ChangeStatus::Unknown:
        return {};
    default:
        qCWarning(KOMMITLIB_LOG) << "Unknown icon" << (int)status;
    }
    return QIcon::fromTheme(QStringLiteral("git-status-update"));
}

} // namespace Git
