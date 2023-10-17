/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchestest.h"
#include "testcommon.h"

#include <QTest>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(BranchesTest)

BranchesTest::BranchesTest(QObject *parent)
    : QObject{parent}
{
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
    auto ok = mManager->createBranch(newBranchName);
    QVERIFY(ok);

    QVERIFY(mManager->branchesNames(Git::Manager::BranchType::LocalBranch).contains(newBranchName));

    auto newBranch = mManager->branch(newBranchName);
    QVERIFY(newBranch != nullptr);
}

void BranchesTest::switchToNewBranch()
{
    auto ok = mManager->switchBranch(newBranchName);
    QVERIFY(ok);
    QCOMPARE(mManager->currentBranch(), newBranchName);
}

void BranchesTest::removeCurrentBranch()
{
    auto ok = mManager->removeBranch(newBranchName);
    QVERIFY(!ok); // we can't remove current branch
}

void BranchesTest::switchToMaster()
{
    auto ok = mManager->switchBranch("master");
    QVERIFY(ok);
}

void BranchesTest::removeNewBranch()
{
    auto ok = mManager->removeBranch(newBranchName);
    QVERIFY(ok);
}

#include "moc_branchestest.cpp"
