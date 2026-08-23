/*
SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "runnerdialogtest.h"

#include "dialogs/runnerdialog.h"

#include <Kommit/Repository>

#include <QProcess>
#include <QPushButton>
#include <QTest>

QTEST_MAIN(RunnerDialogTest)

namespace
{
bool runGit(const QString &path, const QStringList &args)
{
    QProcess process;
    process.setProgram(QStringLiteral("git"));
    process.setArguments(args);
    process.setWorkingDirectory(path);
    process.start();

    return process.waitForFinished() && process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
}

/**
 * A git command that starts and then waits, since it reads what to answer from its input.
 *
 * A command that is over in a moment cannot say what happens to a window closed while one is
 * still going, which is what these tests are about.
 */
const QStringList waitingCommand{QStringLiteral("cat-file"), QStringLiteral("--batch")};
}

RunnerDialogTest::RunnerDialogTest(QObject *parent)
    : QObject{parent}
{
}

void RunnerDialogTest::initTestCase()
{
    QVERIFY(mDir.isValid());
    QVERIFY(runGit(mDir.path(), {QStringLiteral("init"), QStringLiteral("--quiet")}));

    mRepository = new Git::Repository{this};
    QVERIFY(mRepository->open(mDir.path()));
}

void RunnerDialogTest::shouldSurviveBeingClosedWhileGitIsStillRunning()
{
    {
        RunnerDialog dialog{mRepository};
        dialog.run(waitingCommand);
        dialog.close();
    }

    // Whatever the process had left to say arrives after the window is gone, and there has to
    // be nothing left of it listening.
    QTest::qWait(200);
}

void RunnerDialogTest::shouldSurviveBeingStoppedWhileGitIsStillRunning()
{
    {
        RunnerDialog dialog{mRepository};
        dialog.run(waitingCommand);
        dialog.findChild<QDialogButtonBox *>()->button(QDialogButtonBox::Cancel)->click();
    }

    QTest::qWait(200);
}
