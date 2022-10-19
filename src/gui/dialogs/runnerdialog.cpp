/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "runnerdialog.h"

#include "gitmanager.h"

#include <KLocalizedString>
#include <KMessageBox>

#include "gitklient_appdebug.h"

RunnerDialog::RunnerDialog(QWidget *parent)
    : AppDialog(parent)
    , mGitProcess(new QProcess{this})
{
    setupUi(this);

    mGitProcess->setProgram(QStringLiteral("git"));
    mGitProcess->setWorkingDirectory(Git::Manager::instance()->path());

    connect(mGitProcess, &QProcess::readyReadStandardOutput, this, &RunnerDialog::git_readyReadStandardOutput);
    connect(mGitProcess, &QProcess::readyReadStandardError, this, &RunnerDialog::git_readyReadStandardError);

    connect(mGitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RunnerDialog::git_finished);
}

void RunnerDialog::run(const QStringList &args)
{
    progressBar->hide();
    mMode = RunByArgs;
    lineEditCommand->setText(QStringLiteral("git ") + args.join(QLatin1Char(' ')));
    textBrowser->append(lineEditCommand->text());

    textBrowser->append(QStringLiteral("$ ") + lineEditCommand->text());
    mGitProcess->setArguments(args);
    mGitProcess->start();
}

void RunnerDialog::run(Git::AbstractCommand *command)
{
    mMode = RunByCommand;
    if (command->supportWidget()) {
        auto w = command->createWidget();
        tabWidget->insertTab(0, w, i18n("View"));
        tabWidget->setCurrentIndex(0);
    }

    const auto args = command->generateArgs();
    lineEditCommand->setText(QStringLiteral("git ") + args.join(QLatin1Char(' ')));

    textBrowser->append(lineEditCommand->text());
    if (command->supportProgress()) {
        progressBar->show();
        connect(command, &Git::AbstractCommand::progressChanged, progressBar, &QProgressBar::setValue);
    } else {
        progressBar->hide();
    }
    mGitProcess->setArguments(args);
    mGitProcess->start();
    mCmd = command;

    mTimer.start();
}

void RunnerDialog::git_readyReadStandardOutput()
{
    const auto buffer = mGitProcess->readAllStandardOutput();
    qCDebug(GITKLIENT_LOG) << "OUT" << buffer;
    //    textBrowser->setTextColor(Qt::black);
    textBrowser->append(buffer);

    if (mCmd)
        mCmd->parseOutput(buffer, QByteArray());
}

void RunnerDialog::git_readyReadStandardError()
{
    const auto buffer = mGitProcess->readAllStandardError();
    qCDebug(GITKLIENT_LOG) << "ERROR" << buffer;
    //    textBrowser->setTextColor(Qt::red);
    textBrowser->append(buffer);

    if (mCmd)
        mCmd->parseOutput(QByteArray(), buffer);
}

void RunnerDialog::git_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    pushButton->setText(i18n("Close"));

    if (exitStatus == QProcess::CrashExit)
        KMessageBox::error(this, i18n("The git process crashed"));

    if (mCmd && mCmd->status() == Git::AbstractCommand::Error)
        KMessageBox::error(this, mCmd->errorMessage());

    textBrowser->append(
        QStringLiteral("Process finished: (Elapsed time: %1)").arg(QTime::fromMSecsSinceStartOfDay(mTimer.elapsed()).toString(QStringLiteral("HH:mm:ss"))));
}
