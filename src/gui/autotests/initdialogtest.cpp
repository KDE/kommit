/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "initdialogtest.h"
#include "dialogs/initdialog.h"
#include <QTest>
QTEST_MAIN(InitDialogTest)
InitDialogTest::InitDialogTest(QObject *parent)
    : QObject{parent}
{
}

void InitDialogTest::shouldHaveDefaultValues()
{
    InitDialog d(nullptr);
    QVERIFY(d.path().isEmpty());
    auto buttonBox = d.findChild<QDialogButtonBox *>(QStringLiteral("buttonBox"));
    QVERIFY(buttonBox);
    QVERIFY(!buttonBox->button(QDialogButtonBox::Ok)->isEnabled());

    d.setPath(QStringLiteral("bla"));
    QVERIFY(buttonBox->button(QDialogButtonBox::Ok)->isEnabled());
}

#include "moc_initdialogtest.cpp"
