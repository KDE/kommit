/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchestest.h"
#include "testcommon.h"

#include <QTest>
#include <caches/branchescache.h>
#include <entities/branch.h>
#include <repository.h>

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
    mManager = new Git::Repository;
    QVERIFY(!mManager->isValid());

    auto ok = mManager->init(path);
    QVERIFY(ok);
    QCOMPARE(path, mManager->path());

    TestCommon::initSignature(mManager);

    // Reading HEAD throws an error on an empty repo with no commits
    TestCommon::touch(mManager->path() + "/README.md");
    mManager->addFile("README.md");
    mManager->commit("commit1");
    initialBranchName = mManager->branches()->currentName();
}

void BranchesTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
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

void BranchesTest::switchToInitialBranch()
{
    auto ok = mManager->switchBranch(initialBranchName);
    QVERIFY(ok);
}

void BranchesTest::removeNewBranch()
{
    auto branch = mManager->branches()->findByName(newBranchName);
    auto ok = mManager->branches()->remove(branch);
    QVERIFY(ok);
}

void BranchesTest::shouldRemoveABranchThatWasOnlyRead()
{
    const QString name{QStringLiteral("read_branch_name")};
    QVERIFY(mManager->branches()->create(name));

    // A repository opened again knows of the branch only what it reads, which is the way a
    // list of branches on screen comes by them: one after another, not looked up by name.
    Git::Repository other;
    QVERIFY(other.open(mManager->path()));

    const auto branches = other.branches()->allBranches(Git::BranchType::LocalBranch);
    const auto found = std::find_if(branches.begin(), branches.end(), [&name](const Git::Branch &branch) {
        return branch.name() == name;
    });
    QVERIFY(found != branches.end());

    QVERIFY(other.branches()->remove(*found));
    QVERIFY(!other.branches()->names(Git::BranchType::LocalBranch).contains(name));
}

#include "moc_branchestest.cpp"
