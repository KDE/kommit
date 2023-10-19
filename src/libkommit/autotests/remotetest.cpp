/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotetest.h"
#include "testcommon.h"

#include <QTest>
#include <gitmanager.h>
#include <observers/fetchobserver.h>

QTEST_GUILESS_MAIN(RemoteTest)

RemoteTest::RemoteTest(QObject *parent)
    : QObject{parent}
{
}

void RemoteTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    QVERIFY(!mManager->isValid());

    auto ok = mManager->init(path);
    QVERIFY(ok);
    QCOMPARE(path, mManager->path());

    TestCommon::initSignature(mManager);
}

void RemoteTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void RemoteTest::addRemote()
{
    auto ok = mManager->addRemote("origin", "https://invent.kde.org/sdk/kommit.git");
    QVERIFY(ok);

    QVERIFY(mManager->remotes().contains("origin"));
}

void RemoteTest::fetch()
{
    auto observer = new Git::FetchObserver;
    auto ok = mManager->fetch("origin", observer);

    QVERIFY(ok);
    QCOMPARE(observer->receivedObjects(), observer->totalObjects());
}

void RemoteTest::renameRemote()
{
    auto ok = mManager->renameRemote("origin", "origin2");
    QVERIFY(ok);

    QVERIFY(!mManager->remotes().contains("origin"));
    QVERIFY(mManager->remotes().contains("origin2"));
}

void RemoteTest::removeRemote()
{
    auto ok = mManager->removeRemote("origin2");
    QVERIFY(ok);
}

void RemoteTest::pull()
{
}

#include "moc_remotetest.cpp"
