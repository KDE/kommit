/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandpushtest.h"
#include "commands/commandpush.h"
#include <QTest>
QTEST_MAIN(CommandPushTest)
CommandPushTest::CommandPushTest(QObject *parent)
    : QObject{parent}
{
}

void CommandPushTest::shouldHaveDefaultValues()
{
    Git::CommandPush command;

    QVERIFY(!command.force());
    QVERIFY(command.remote().isEmpty());
    QVERIFY(command.localBranch().isEmpty());
    QVERIFY(command.remoteBranch().isEmpty());
}
