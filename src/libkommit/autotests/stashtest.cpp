/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashtest.h"
#include "caches/stashescache.h"
#include "entities/index.h"
#include "entities/stash.h"
#include "repository.h"
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
    mManager = new Git::Repository;

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

    auto index = mManager->index();
    index.addByPath("README.md");
    index.writeTree();
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
    auto ok = mManager->stashes()->create("stash1");
    QVERIFY(ok);

    auto changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.count(), 0);

    auto stashes = mManager->stashes()->allStashes();
    QCOMPARE(stashes.count(), 1);
    QCOMPARE(stashes.at(0).message(), QStringLiteral("On master: stash1"));
}

void StashTest::tryToApplyInChangedWorkDir()
{
    TestCommon::touch(mManager->path() + "/README.md");
    auto ok = mManager->stashes()->apply(QStringLiteral("On master: stash1"));
    QVERIFY(!ok); // we have un committed changes
}

void StashTest::revertAndApplyStash()
{
    TestCommon::touch(mManager->path() + "/README.md");
    auto ok = mManager->revertFile("README.md");
    QVERIFY(ok);
    ok = mManager->stashes()->apply(QStringLiteral("On master: stash1"));
    QVERIFY(ok);
}

void StashTest::commitAndApplyStash()
{
    TestCommon::touch(mManager->path() + "/README.md");

    auto index = mManager->index();
    index.addByPath("README.md");
    index.writeTree();
    mManager->commit("commit2");

    auto ok = mManager->stashes()->apply(QStringLiteral("On master: stash1"));
    QVERIFY(ok);
}

#include "moc_stashtest.cpp"
