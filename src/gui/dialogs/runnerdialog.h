/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QElapsedTimer>

#include "core/appdialog.h"
#include "ui_runnerdialog.h"

#include <QProcess>

namespace Git
{
class AbstractCommand;
class Manager;
}
class RunnerDialog : public AppDialog, private Ui::RunnerDialog
{
    Q_OBJECT

public:
    explicit RunnerDialog(Git::Manager *git, QWidget *parent = nullptr);
    void run(const QStringList &args);
    void run(Git::AbstractCommand *command);

    bool autoClose() const;
    void setAutoClose(bool newAutoClose);

private Q_SLOTS:
    void git_readyReadStandardOutput();
    void git_readyReadStandardError();
    void git_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotPushButtonCloseClicked();
    void slotPushButtonStopClicked();

private:
    QProcess *mGitProcess = nullptr;
    Git::Manager *mGit = nullptr;

    enum Mode { None, RunByArgs, RunByCommand };
    Mode mMode{None};
    Git::AbstractCommand *mCmd{nullptr};

    QElapsedTimer mTimer;

    bool mAutoClose{false};

    QByteArray mStandardOutput;
    QByteArray mErrorOutput;
};
