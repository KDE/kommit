/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchestest.h"
#include "testcommon.h"

#include <QTest>
#include <caches/branchescache.h>
#include <entities/branch.h>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(BranchesTest)

BranchesTest::BranchesTest(QObject *parent)
    : QObject{parent}
{
}

BranchesTest::~BranchesTest()
{
    delete mManager;
}

void BranchesTest::initTestCase()
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

void BranchesTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void BranchesTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit1");
}

void BranchesTest::createBranch()
{
    auto ok = mManager->branches()->create(newBranchName);
    QVERIFY(ok);

    QVERIFY(mManager->branches()->names(Git::BranchType::LocalBranch).contains(newBranchName));

    auto newBranch = mManager->branches()->findByName(newBranchName);
    QVERIFY(!newBranch.isNull());
}

void BranchesTest::switchToNewBranch()
{
    auto ok = mManager->switchBranch(newBranchName);
    QVERIFY(ok);
    QCOMPARE(mManager->branches()->currentName(), newBranchName);
}

void BranchesTest::removeCurrentBranch()
{
    auto branch = mManager->branches()->findByName(newBranchName);
    auto ok = mManager->branches()->remove(branch);
    QVERIFY(!ok); // we can't remove current branch
}

void BranchesTest::switchToMaster()
{
    auto ok = mManager->switchBranch("master");
    QVERIFY(ok);
}

void BranchesTest::removeNewBranch()
{
    auto branch = mManager->branches()->findByName(newBranchName);
    auto ok = mManager->branches()->remove(branch);
    QVERIFY(ok);
}

#include "moc_branchestest.cpp"
