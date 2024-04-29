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
    QTest::addColumn<QString>("url");
    QTest::addColumn<QString>("localPath");
    QTest::addColumn<QString>("result");
    QTest::addRow("empty") << QString() << QString() << QString();
    QTest::addRow("pathwithoutgit") << QStringLiteral("http://bla.com/bla") << QStringLiteral("/path/to/a/dir") << QStringLiteral("/path/to/a/dir/bla");
    QTest::addRow("pathwitgit") << QStringLiteral("http://bla.com/bla.git") << QStringLiteral("/path/to/a/dir") << QStringLiteral("/path/to/a/dir/bla");
}

void CloneDialogTest::shouldChangePath()
{
    QFETCH(QString, url);
    QFETCH(QString, localPath);
    QFETCH(QString, result);

    CloneDialog d;
    auto lineEditPath = d.findChild<KUrlRequester *>(QStringLiteral("lineEditPath"));
    auto lineEditUrl = d.findChild<QLineEdit *>(QStringLiteral("lineEditUrl"));

    d.setLocalPath(localPath);
    lineEditUrl->setText(url);

    QCOMPARE(lineEditPath->text(), result);
}

#include "moc_clonedialogtest.cpp"
