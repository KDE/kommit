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

    TestCommon::initSignature(mManager);
}

void StashTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void StashTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit1");
}

void StashTest::touchAFile()
{
    mFileContentInStash = TestCommon::touch(mManager->path() + "/README.md");
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

    auto stashes = mManager->stashes();
    QCOMPARE(stashes.count(), 1);
    QCOMPARE(stashes.at(0)->name(), QStringLiteral("On master: stash1"));
}

void StashTest::tryToApplyInChangedWorkDir()
{
    TestCommon::touch(mManager->path() + "/README.md");
    auto ok = mManager->applyStash(QStringLiteral("On master: stash1"));
    QVERIFY(!ok); // we have un committed changes
}

void StashTest::revertAndApplyStash()
{
    TestCommon::touch(mManager->path() + "/README.md");
    auto ok = mManager->revertFile("README.md");
    QVERIFY(ok);
    ok = mManager->applyStash(QStringLiteral("On master: stash1"));
    QVERIFY(ok);
}

void StashTest::commitAndApplyStash()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit2");

    auto ok = mManager->applyStash(QStringLiteral("On master: stash1"));
    QVERIFY(ok);
}

#include "moc_stashtest.cpp"
