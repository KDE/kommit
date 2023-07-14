/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "runnerdialog.h"

#include "gitmanager.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <QInputDialog>

#include "kommit_appdebug.h"

RunnerDialog::RunnerDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(parent)
    , mGitProcess(new QProcess{this})
    , mGit(git)
{
    setupUi(this);

    mGitProcess->setProgram(QStringLiteral("git"));
    mGitProcess->setWorkingDirectory(git->path());

    connect(mGitProcess, &QProcess::readyReadStandardOutput, this, &RunnerDialog::git_readyReadStandardOutput);
    connect(mGitProcess, &QProcess::readyReadStandardError, this, &RunnerDialog::git_readyReadStandardError);
    connect(pushButtonStop, &QAbstractButton::clicked, this, &RunnerDialog::slotPushButtonStopClicked);
    connect(pushButtonClose, &QAbstractButton::clicked, this, &RunnerDialog::slotPushButtonCloseClicked);

    connect(mGitProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &RunnerDialog::git_finished);

    pushButtonStop->hide();
    pushButtonClose->show();
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

    pushButtonStop->show();
    pushButtonClose->hide();
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

    pushButtonStop->show();
    pushButtonClose->hide();
}

void RunnerDialog::git_readyReadStandardOutput()
{
    const auto buffer = mGitProcess->readAllStandardOutput();
    mErrorOutput.append(buffer);
    textBrowser->append(buffer);

    if (mCmd && mCmd->supportProgress())
        mCmd->parseOutputSection(buffer, QByteArray());

    QRegularExpression r{"Username for '.*': "};

    if (r.match(buffer).hasMatch()) {
        QInputDialog::getText(this, buffer, "Username:");
    }
}

void RunnerDialog::git_readyReadStandardError()
{
    const auto buffer = mGitProcess->readAllStandardError();
    mStandardOutput.append(buffer);
    textBrowser->append(buffer);

    if (mCmd && mCmd->supportProgress())
        mCmd->parseOutputSection(QByteArray(), buffer);
}

void RunnerDialog::git_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    pushButtonStop->hide();
    pushButtonClose->show();

    if (mCmd)
        mCmd->parseOutputSection(mStandardOutput, mErrorOutput);

    bool isSuccessful;
    if (mCmd)
        isSuccessful = mCmd->parseOutput(mStandardOutput, mErrorOutput);
    else
        isSuccessful = exitStatus == QProcess::NormalExit;

    QString exitMessage;

    if (isSuccessful) {
        exitMessage = i18n("Process finished");
    } else {
        if (mCmd)
            KMessageBox::error(this, mCmd->errorMessage());
        else
            KMessageBox::error(this, i18n("The git process crashed"));
        exitMessage = i18n("Process finished with error");
    }

    textBrowser->append(
        QStringLiteral("%1: (Elapsed time: %2)").arg(exitMessage, QTime::fromMSecsSinceStartOfDay(mTimer.elapsed()).toString(QStringLiteral("HH:mm:ss"))));

    if (mAutoClose) {
        if (isSuccessful)
            accept();
        else
            setResult(QDialog::Rejected);
    }
}

void RunnerDialog::slotPushButtonCloseClicked()
{
    close();
}

void RunnerDialog::slotPushButtonStopClicked()
{
    if (mGitProcess->isOpen())
        mGitProcess->kill();
    close();
}

bool RunnerDialog::autoClose() const
{
    return mAutoClose;
}

void RunnerDialog::setAutoClose(bool newAutoClose)
{
    mAutoClose = newAutoClose;
}

#include "moc_runnerdialog.cpp"
