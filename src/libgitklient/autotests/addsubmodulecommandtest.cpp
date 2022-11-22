/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "addsubmodulecommandtest.h"
#include "commands/addsubmodulecommand.h"
#include <QTest>
QTEST_MAIN(AddSubmoduleCommandTest)
AddSubmoduleCommandTest::AddSubmoduleCommandTest(QObject *parent)
    : QObject{parent}
{
}

void AddSubmoduleCommandTest::shouldHaveDefaultValues()
{
    Git::AddSubmoduleCommand command(nullptr);
    QVERIFY(command.localPath().isEmpty());
    QVERIFY(command.branch().isEmpty());
    QVERIFY(!command.force());
}
