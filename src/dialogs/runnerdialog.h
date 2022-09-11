/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_runnerdialog.h"

#include <QProcess>

namespace Git
{
class AbstractCommand;
}
class QProcess;
class RunnerDialog : public AppDialog, private Ui::RunnerDialog
{
    Q_OBJECT

    QProcess *_git;

    enum Mode { None, RunByArgs, RunByCommand };
    Mode _mode{None};
    Git::AbstractCommand *_cmd{nullptr};

public:
    explicit RunnerDialog(QWidget *parent = nullptr);
    void run(const QStringList &args);
    void run(Git::AbstractCommand *command);

private Q_SLOTS:
    void git_readyReadStandardOutput();
    void git_readyReadStandardError();
    void git_finished(int exitCode, QProcess::ExitStatus exitStatus);
};
