/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clonecommandtest.h"
#include "commands/commandclone.h"
#include <QTest>
QTEST_MAIN(CloneCommandTest)
CloneCommandTest::CloneCommandTest(QObject *parent)
    : QObject(parent)
{
}

void CloneCommandTest::shouldHaveDefaultValues()
{
    Git::CloneCommand command;
    QVERIFY(command.repoUrl().isEmpty());
    QVERIFY(command.origin().isEmpty());
    QVERIFY(command.localPath().isEmpty());
    QVERIFY(command.branch().isEmpty());
    QCOMPARE(command.depth(), -1);
    QVERIFY(!command.recursive());
}

void CloneCommandTest::shouldGenerateCommand_data()
{
    QTest::addColumn<QString>("newRepoUrl");
    QTest::addColumn<QString>("newLocalPath");
    QTest::addColumn<QString>("newBranch");
    QTest::addColumn<QString>("newOrigin");
    QTest::addColumn<int>("newDepth");
    QTest::addColumn<bool>("newRecursive");
    QTest::addColumn<QStringList>("result");
    {
        const QStringList lst{QStringLiteral("clone"), QStringLiteral("--progress"), QString(), QString()};
        QTest::addRow("empty") << QString() << QString() << QString() << QString() << -1 << false << lst;
    }
    {
        const QString newRepoUrl = QStringLiteral("bla");
        const QStringList lst{QStringLiteral("clone"), QStringLiteral("--progress"), newRepoUrl, QString()};
        QTest::addRow("newRepo") << newRepoUrl << QString() << QString() << QString() << -1 << false << lst;
    }
    {
        const QString newRepoUrl = QStringLiteral("bla");
        const int depth = 5;
        const QStringList lst{QStringLiteral("clone"), QStringLiteral("--progress"), newRepoUrl, QString(), QStringLiteral("--depth"), QString::number(depth)};
        QTest::addRow("depth") << newRepoUrl << QString() << QString() << QString() << depth << false << lst;
    }
}

void CloneCommandTest::shouldGenerateCommand()
{
    QFETCH(QString, newRepoUrl);
    QFETCH(QString, newLocalPath);
    QFETCH(QString, newBranch);
    QFETCH(QString, newOrigin);
    QFETCH(int, newDepth);
    QFETCH(bool, newRecursive);
    QFETCH(QStringList, result);
    Git::CloneCommand command;
    command.setRepoUrl(newRepoUrl);
    command.setLocalPath(newLocalPath);
    command.setBranch(newBranch);
    command.setDepth(newDepth);
    command.setOrigin(newOrigin);
    command.setRecursive(newRecursive);
    QCOMPARE(command.generateArgs(), result);
}
