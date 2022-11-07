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
