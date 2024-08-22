/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QElapsedTimer>

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_runnerdialog.h"

#include <QProcess>

namespace Git
{
class AbstractCommand;
class Manager;
}
class LIBKOMMITWIDGETS_EXPORT RunnerDialog : public AppDialog, private Ui::RunnerDialog
{
    Q_OBJECT

public:
    explicit RunnerDialog(Git::Manager *git, QWidget *parent = nullptr);
    ~RunnerDialog() override;
    void run(const QStringList &args);
    void run(Git::AbstractCommand *command);

    [[nodiscard]] bool autoClose() const;
    void setAutoClose(bool newAutoClose);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void git_readyReadStandardOutput();
    LIBKOMMITWIDGETS_NO_EXPORT void git_readyReadStandardError();
    LIBKOMMITWIDGETS_NO_EXPORT void git_finished(int exitCode, QProcess::ExitStatus exitStatus);
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonCloseClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonStopClicked();

    QProcess *mGitProcess = nullptr;

    enum Mode {
        None,
        RunByArgs,
        RunByCommand,
    };
    Mode mMode{None};
    Git::AbstractCommand *mCmd{nullptr};

    QElapsedTimer mTimer;

    bool mAutoClose{false};

    QByteArray mStandardOutput;
    QByteArray mErrorOutput;
};
