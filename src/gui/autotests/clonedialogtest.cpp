/*
    SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#include "clonedialogtest.h"
#include "dialogs/clonedialog.h"
#include <QStandardPaths>
#include <QTest>
QTEST_MAIN(CloneDialogTest)
CloneDialogTest::CloneDialogTest(QObject *parent)
    : QObject{parent}
{
    QStandardPaths::setTestModeEnabled(true);
}

void CloneDialogTest::shouldHaveDefaultValues()
{
    CloneDialog d;
    auto lineEditPath = d.findChild<KUrlRequester *>(QStringLiteral("lineEditPath"));
    QVERIFY(lineEditPath);
    QVERIFY(lineEditPath->text().isEmpty());

    auto lineEditUrl = d.findChild<QLineEdit *>(QStringLiteral("lineEditUrl"));
    QVERIFY(lineEditUrl);
    QVERIFY(lineEditUrl->text().isEmpty());
}

void CloneDialogTest::shouldChangePath_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("result");
    QTest::addRow("empty") << QString() << QString();
    QTest::addRow("pathwithoutgit") << QStringLiteral("/bla/bla/bli") << QStringLiteral("/bla/bla/bli");
    QTest::addRow("pathwitgit") << QStringLiteral("/bla/bla/bli.git") << QStringLiteral("/bla/bla/bli.git/bli");
}

void CloneDialogTest::shouldChangePath()
{
    QFETCH(QString, path);
    QFETCH(QString, result);

    CloneDialog d;
    auto lineEditPath = d.findChild<KUrlRequester *>(QStringLiteral("lineEditPath"));

    d.setLocalPath(path);
    QCOMPARE(lineEditPath->text(), result);
}

#include "moc_clonedialogtest.cpp"
