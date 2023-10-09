/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashtest.h"
#include "gitmanager.h"
#include "testcommon.h"
#include <QTest>

QTEST_GUILESS_MAIN(StashTest)

StashTest::StashTest(QObject *parent)
    : QObject{parent}
{
}

void StashTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    auto init = mManager->init(path);
    QCOMPARE(mManager->path(), path);
    QVERIFY(init);
    QVERIFY(mManager->isValid());
}

void StashTest::cleanupTestCase()
{
    //    TestCommon::cleanPath(mManager);
}

void StashTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit1");
}

void StashTest::touchAFile()
{
    TestCommon::touch(mManager->path() + "/README.md");
    auto changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.count(), 1);
    QCOMPARE(changedFiles.value("README.md"), Git::ChangeStatus::Modified);
}

void StashTest::makeStash()
{
    auto ok = mManager->createStash("stash1");
    QVERIFY(ok);

    auto changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.count(), 0);

    QList<QSharedPointer<Git::Stash>> stashes;
    mManager->forEachStash([&stashes](QSharedPointer<Git::Stash> stash) {
        stashes << stash;
        return 0;
    });
    QCOMPARE(stashes.count(), 1);
    QCOMPARE(stashes.at(0)->name(), QStringLiteral("On master: stash1"));
}

#include "moc_stashtest.cpp"
