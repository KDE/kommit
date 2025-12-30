/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotetest.h"
#include "caches/remotescache.h"
#include "testcommon.h"

#include <Kommit/FetchOptions>
#include <QTest>
#include <repository.h>

QTEST_GUILESS_MAIN(RemoteTest)

RemoteTest::RemoteTest(QObject *parent)
    : QObject{parent}
{
}

RemoteTest::~RemoteTest()
{
}

void RemoteTest::initTestCase()
{
    auto path = mDir.path();
    mManager = new Git::Repository{this};
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
    auto ok = mManager->remotes()->create("origin", "https://invent.kde.org/sdk/kommit.git");
    QVERIFY(ok);

    QVERIFY(mManager->remotes()->allNames().contains("origin"));
}

void RemoteTest::fetch()
{
    Git::FetchOptions fetchOptions{mManager};

    auto remote = mManager->remotes()->findByName("origin");
    QVERIFY(!remote.isNull());

    auto ok = mManager->fetch(remote, Git::Branch{}, &fetchOptions);
    QVERIFY(ok);
}

void RemoteTest::renameRemote()
{
    auto origin = mManager->remotes()->findByName("origin");
    auto ok = mManager->remotes()->rename(origin, "origin2");
    QVERIFY(ok);

    auto remotes = mManager->remotes()->allNames();

    QVERIFY(!remotes.contains("origin"));
    QVERIFY(remotes.contains("origin2"));
}

void RemoteTest::removeRemote()
{
    auto ok = mManager->remotes()->remove("origin2");
    QVERIFY(ok);
}

void RemoteTest::pull()
{
}

#include "moc_remotetest.cpp"
