/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commandcleantest.h"
#include "commands/commandclean.h"

#include <QTest>
QTEST_MAIN(CommandCleanTest)
CommandCleanTest::CommandCleanTest(QObject *parent)
    : QObject{parent}
{
}

void CommandCleanTest::shouldHaveDefaultValues()
{
    Git::CommandClean command;
    // TODO
}
