/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchesdifftest.h"
#include "caches/branchescache.h"
#include "entities/treediff.h"
#include "testcommon.h"

#include <Kommit/CommitsCache>
#include <Kommit/Index>

#include <QTest>
#include <entities/branch.h>
#include <repository.h>

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
    auto path = mDir.path();
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
    TestCommon::touch(mManager, "unchanged");
    TestCommon::touch(mManager, "changed_in_both");
    TestCommon::touch(mManager, "changed_in_master");
    TestCommon::touch(mManager, "changed_in_dev");

    TestCommon::touch(mManager, "inside_dir/unchanged");
    TestCommon::touch(mManager, "inside_dir/changed_in_both");
    TestCommon::touch(mManager, "inside_dir/changed_in_master");
    TestCommon::touch(mManager, "inside_dir/changed_in_dev");

    auto index = mManager->index();

    index.addByPath("unchanged");
    index.addByPath("changed_in_both");
    index.addByPath("changed_in_master");
    index.addByPath("changed_in_dev");
    index.addByPath("inside_dir/unchanged");
    index.addByPath("inside_dir/changed_in_both");
    index.addByPath("inside_dir/changed_in_master");
    index.addByPath("inside_dir/changed_in_dev");
    index.writeTree();

    auto ok = mManager->commit("commit1");
    QVERIFY(ok);
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

    TestCommon::touch(mManager, "changed_in_both");
    TestCommon::touch(mManager, "changed_in_dev");
    TestCommon::touch(mManager, "only_in_dev");
    TestCommon::touch(mManager, "inside_dir/changed_in_both");
    TestCommon::touch(mManager, "inside_dir/changed_in_dev");
    TestCommon::touch(mManager, "inside_dir/only_in_dev");

    auto index = mManager->index();
    index.addByPath("changed_in_both");
    index.addByPath("changed_in_dev");
    index.addByPath("only_in_dev");
    index.addByPath("inside_dir/changed_in_both");
    index.addByPath("inside_dir/changed_in_dev");
    index.addByPath("inside_dir/only_in_dev");
    index.writeTree();

    ok = mManager->commits()->create("commit_in_dev");
    QVERIFY(ok);
}

void BranchesDiffTest::switchToMaster()
{
    auto ok = mManager->switchBranch("master");
    QVERIFY(ok);

    TestCommon::touch(mManager, "changed_in_both");
    TestCommon::touch(mManager, "changed_in_master");
    TestCommon::touch(mManager, "only_in_master");

    TestCommon::touch(mManager, "inside_dir/changed_in_both");
    TestCommon::touch(mManager, "inside_dir/changed_in_master");
    TestCommon::touch(mManager, "inside_dir/only_in_master");

    auto index = mManager->index();
    index.addByPath("changed_in_both");
    index.addByPath("changed_in_master");
    index.addByPath("only_in_master");
    index.addByPath("inside_dir/changed_in_both");
    index.addByPath("inside_dir/changed_in_master");
    index.addByPath("inside_dir/only_in_master");
    index.writeTree();

    ok = mManager->commits()->create("commit_in_master");
    QVERIFY(ok);
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

    QVERIFY((diff.status("unchanged") & Git::DeltaFlag::Unmodified) == Git::DeltaFlag::Unmodified);
    QVERIFY((diff.status("changed_in_both") & Git::DeltaFlag::Modified) == Git::DeltaFlag::Modified);
    QVERIFY((diff.status("changed_in_master") & Git::DeltaFlag::Modified) == Git::DeltaFlag::Modified);
    QVERIFY((diff.status("changed_in_dev") & Git::DeltaFlag::Modified) == Git::DeltaFlag::Modified);
    QVERIFY((diff.status("only_in_master") & Git::DeltaFlag::Deleted) == Git::DeltaFlag::Deleted);
    QVERIFY((diff.status("only_in_dev") & Git::DeltaFlag::Added) == Git::DeltaFlag::Added);

    QVERIFY((diff.status("inside_dir/unchanged") & Git::DeltaFlag::Unmodified) == Git::DeltaFlag::Unmodified);
    QVERIFY((diff.status("inside_dir/changed_in_both") & Git::DeltaFlag::Modified) == Git::DeltaFlag::Modified);
    QVERIFY((diff.status("inside_dir/changed_in_master") & Git::DeltaFlag::Modified) == Git::DeltaFlag::Modified);
    QVERIFY((diff.status("inside_dir/changed_in_dev") & Git::DeltaFlag::Modified) == Git::DeltaFlag::Modified);
    QVERIFY((diff.status("inside_dir/only_in_master") & Git::DeltaFlag::Deleted) == Git::DeltaFlag::Deleted);
    QVERIFY((diff.status("inside_dir/only_in_dev") & Git::DeltaFlag::Added) == Git::DeltaFlag::Added);
}

#include "moc_branchesdifftest.cpp"
