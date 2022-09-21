/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "runnerdialog.h"

#include "git/gitmanager.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QDebug>

RunnerDialog::RunnerDialog(QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);

    mGit = new QProcess{this};
    mGit->setProgram(QStringLiteral("git"));
    mGit->setWorkingDirectory(Git::Manager::instance()->path());

    connect(mGit, &QProcess::readyReadStandardOutput, this, &RunnerDialog::git_readyReadStandardOutput);
    connect(mGit, &QProcess::readyReadStandardError, this, &RunnerDialog::git_readyReadStandardError);

    connect(mGit, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RunnerDialog::git_finished);
}

void RunnerDialog::run(const QStringList &args)
{
    progressBar->hide();
    mMode = RunByArgs;
    lineEditCommand->setText("git " + args.join(QLatin1Char(' ')));
    textBrowser->append("$ " + lineEditCommand->text());
    mGit->setArguments(args);
    mGit->start();
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
    lineEditCommand->setText("git " + args.join(QLatin1Char(' ')));

    if (command->supportProgress()) {
        progressBar->show();
        connect(command, &Git::AbstractCommand::progressChanged, progressBar, &QProgressBar::setValue);
    } else {
        progressBar->hide();
    }
    mGit->setArguments(args);
    mGit->start();
    mCmd = command;
}

void RunnerDialog::git_readyReadStandardOutput()
{
    auto buffer = mGit->readAllStandardOutput();
    qDebug() << "OUT" << buffer;
    //    textBrowser->setTextColor(Qt::black);
    textBrowser->append(buffer);

    if (mCmd)
        mCmd->parseOutput(buffer, QByteArray());
}

void RunnerDialog::git_readyReadStandardError()
{
    auto buffer = mGit->readAllStandardError();
    qDebug() << "ERROR" << buffer;
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
}
