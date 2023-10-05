/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clonetest.h"
#include "testcommon.h"

#include <QTest>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(CloneTest)

CloneTest::CloneTest(QObject *parent)
    : QObject{parent}
{
}

void CloneTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    QVERIFY(!mManager->isValid());
}

void CloneTest::clone()
{
    auto path = TestCommon::getTempPath();
    auto ok = mManager->clone("https://invent.kde.org/sdk/kommit.git", path);
    QVERIFY(ok);

    path.append("/");
    QCOMPARE(path, mManager->path());

    auto tags = mManager->tags();
    QVERIFY(tags.contains("v1.0.2"));
}

void CloneTest::cleanupTestCase()
{
}

#include "moc_clonetest.cpp"
