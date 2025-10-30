/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesdifftest.h"
#include "caches/branchescache.h"
#include "entities/treediff.h"
#include "testcommon.h"

#include <QTest>
#include <entities/branch.h>
#include <repository.h>
#include <Kommit/CommitsCache>

QTEST_GUILESS_MAIN(BranchesDiffTest)

BranchesDiffTest::BranchesDiffTest(QObject *parent)
    : QObject{parent}
{
}

BranchesDiffTest::~BranchesDiffTest()
{
    delete mManager;
}

void BranchesDiffTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Repository;
    QVERIFY(!mManager->isValid());

    auto ok = mManager->init(path);
    QVERIFY(ok);
    QCOMPARE(path, mManager->path());

    TestCommon::initSignature(mManager);
}

void BranchesDiffTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void BranchesDiffTest::makeACommit()
{
    TestCommon::touch(mManager, "/unchanged");
    TestCommon::touch(mManager, "/changed_in_both");
    TestCommon::touch(mManager, "/changed_in_master");
    TestCommon::touch(mManager, "/changed_in_dev");

    TestCommon::touch(mManager, "/inside_dir/unchanged");
    TestCommon::touch(mManager, "/inside_dir/changed_in_both");
    TestCommon::touch(mManager, "/inside_dir/changed_in_master");
    TestCommon::touch(mManager, "/inside_dir/changed_in_dev");

    mManager->commits()->create("commit1");
}

void BranchesDiffTest::createBranch()
{
    auto ok = mManager->branches()->create(mNewBranchName);
    QVERIFY(ok);

    QVERIFY(mManager->branches()->names(Git::BranchType::LocalBranch).contains(mNewBranchName));

    auto newBranch = mManager->branches()->findByName(mNewBranchName);
    QVERIFY(!newBranch.isNull());
}

void BranchesDiffTest::switchToNewBranch()
{
    auto ok = mManager->switchBranch(mNewBranchName);
    QVERIFY(ok);
    QCOMPARE(mManager->branches()->currentName(), mNewBranchName);

    TestCommon::touch(mManager, "/changed_in_both");
    TestCommon::touch(mManager, "/changed_in_dev");
    TestCommon::touch(mManager, "/only_in_dev");

    TestCommon::touch(mManager, "/inside_dir/changed_in_both");
    TestCommon::touch(mManager, "/inside_dir/changed_in_dev");
    TestCommon::touch(mManager, "/inside_dir/only_in_dev");

    mManager->commits()->create("commit_in_dev");
}

void BranchesDiffTest::switchToMaster()
{
    auto ok = mManager->switchBranch("master");
    QVERIFY(ok);

    TestCommon::touch(mManager, "/changed_in_both");
    TestCommon::touch(mManager, "/changed_in_master");
    TestCommon::touch(mManager, "/only_in_master");

    TestCommon::touch(mManager, "/inside_dir/changed_in_both");
    TestCommon::touch(mManager, "/inside_dir/changed_in_master");
    TestCommon::touch(mManager, "/inside_dir/only_in_master");

    mManager->commits()->create("commit_in_master");
}

void BranchesDiffTest::diff()
{
    auto branches = mManager->branches()->allBranches(Git::BranchType::LocalBranch);
    QCOMPARE(branches.size(), 2);

    QCOMPARE(branches.at(0).name(), "dev");
    QCOMPARE(branches.at(1).name(), "master");
    auto diff = mManager->diff(branches.at(1).tree(), branches.at(0).tree());

    QCOMPARE(diff.size(), 10);

    QVERIFY(!diff.contains("unchanged"));
    QVERIFY(diff.contains("changed_in_both"));
    QVERIFY(diff.contains("changed_in_master"));
    QVERIFY(diff.contains("changed_in_dev"));
    QVERIFY(diff.contains("only_in_master"));
    QVERIFY(diff.contains("only_in_dev"));

    QCOMPARE(diff.status("unchanged"), Git::ChangeStatus::Unmodified);
    QCOMPARE(diff.status("changed_in_both"), Git::ChangeStatus::Modified);
    QCOMPARE(diff.status("changed_in_master"), Git::ChangeStatus::Modified);
    QCOMPARE(diff.status("changed_in_dev"), Git::ChangeStatus::Modified);
    QCOMPARE(diff.status("only_in_master"), Git::ChangeStatus::Removed);
    QCOMPARE(diff.status("only_in_dev"), Git::ChangeStatus::Added);

    QCOMPARE(diff.status("inside_dir/unchanged"), Git::ChangeStatus::Unmodified);
    QCOMPARE(diff.status("inside_dir/changed_in_both"), Git::ChangeStatus::Modified);
    QCOMPARE(diff.status("inside_dir/changed_in_master"), Git::ChangeStatus::Modified);
    QCOMPARE(diff.status("inside_dir/changed_in_dev"), Git::ChangeStatus::Modified);
    QCOMPARE(diff.status("inside_dir/only_in_master"), Git::ChangeStatus::Removed);
    QCOMPARE(diff.status("inside_dir/only_in_dev"), Git::ChangeStatus::Added);
}

#include "moc_branchesdifftest.cpp"
