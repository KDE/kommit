/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesdifftest.h"
#include "testcommon.h"

#include <QTest>
#include <entities/branch.h>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(BranchesDiffTest)

BranchesDiffTest::BranchesDiffTest(QObject *parent)
    : QObject{parent}
{
}

void BranchesDiffTest::initTestCase()
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

    mManager->commit("commit1");
}

void BranchesDiffTest::createBranch()
{
    auto ok = mManager->createBranch(newBranchName);
    QVERIFY(ok);

    QVERIFY(mManager->branchesNames(Git::Manager::BranchType::LocalBranch).contains(newBranchName));

    auto newBranch = mManager->branch(newBranchName);
    QVERIFY(newBranch != nullptr);
}

void BranchesDiffTest::switchToNewBranch()
{
    auto ok = mManager->switchBranch(newBranchName);
    QVERIFY(ok);
    QCOMPARE(mManager->currentBranch(), newBranchName);

    TestCommon::touch(mManager, "/changed_in_both");
    TestCommon::touch(mManager, "/changed_in_dev");
    TestCommon::touch(mManager, "/only_in_dev");

    TestCommon::touch(mManager, "/inside_dir/changed_in_both");
    TestCommon::touch(mManager, "/inside_dir/changed_in_dev");
    TestCommon::touch(mManager, "/inside_dir/only_in_dev");

    mManager->commit("commit_in_dev");
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

    mManager->commit("commit_in_master");
}

void BranchesDiffTest::diff()
{
    auto branches = mManager->branches(Git::Manager::BranchType::LocalBranch);
    QCOMPARE(branches.size(), 2);

    QCOMPARE(branches.at(0)->name(), "dev");
    QCOMPARE(branches.at(1)->name(), "master");
    auto diff = mManager->diff(branches.at(1).data(), branches.at(0).data());

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
