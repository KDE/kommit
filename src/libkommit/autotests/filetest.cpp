/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filetest.h"
#include "caches/branchescache.h"
#include "caches/commitscache.h"
#include "entities/blob.h"
#include "entities/branch.h"
#include "entities/file.h"
#include "entities/index.h"
#include "entities/tree.h"
#include "repository.h"
#include "testcommon.h"
#include <QTest>

#include <entities/commit.h>
#include <Kommit/CommitsCache>

QTEST_GUILESS_MAIN(FileTest)

FileTest::FileTest(QObject *parent)
    : QObject{parent}
{
}

FileTest::~FileTest()
{
    delete mManager;
}

void FileTest::initTestCase()
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

void FileTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void FileTest::makeFirstCommit()
{
    mFileContentAtFirstCommit = TestCommon::touch(mManager->path() + "/README.md");
    auto index = mManager->index();
    QVERIFY(!index.isNull());
    QVERIFY(index.addByPath("README.md"));
    QVERIFY(index.writeTree());
    mManager->commits()->create("first commit");
}

void FileTest::makeSecondCommit()
{
    mFileContentAtSecondCommit = TestCommon::touch(mManager->path() + "/README.md");
    auto index = mManager->index();
    QVERIFY(!index.isNull());
    QVERIFY(index.addByPath("README.md"));
    QVERIFY(index.writeTree());
    mManager->commits()->create("second commit");
}

void FileTest::makeThirdCommit()
{
    mFileContentAtThirdCommit = TestCommon::touch(mManager->path() + "/README.md");
    auto index = mManager->index();
    QVERIFY(!index.isNull());
    QVERIFY(index.addByPath("README.md"));
    QVERIFY(index.writeTree());
    mManager->commits()->create("third commit");
}

void FileTest::checkContents()
{
    auto commits = mManager->commits()->allCommits();
    QCOMPARE(commits.size(), 3);
    QCOMPARE(commits.at(2).message(), "first commit");
    QCOMPARE(commits.at(1).message(), "second commit");
    QCOMPARE(commits.at(0).message(), "third commit");

    Git::File firstFile{mManager, commits.at(2).commitHash(), "README.md"};
    Git::File secondFile{mManager, commits.at(1).commitHash(), "README.md"};
    Git::File thirdFile{mManager, commits.at(0).commitHash(), "README.md"};

    QCOMPARE(firstFile.content(), mFileContentAtFirstCommit);
    QCOMPARE(secondFile.content(), mFileContentAtSecondCommit);
    QCOMPARE(thirdFile.content(), mFileContentAtThirdCommit);

    QCOMPARE(commits.at(2).tree().file("README.md").content(), mFileContentAtFirstCommit);
    QCOMPARE(commits.at(1).tree().file("README.md").content(), mFileContentAtSecondCommit);
    QCOMPARE(commits.at(0).tree().file("README.md").content(), mFileContentAtThirdCommit);
}

void FileTest::checkContentInBranch()
{
    auto branches = mManager->branches()->allBranches(Git::BranchType::LocalBranch);
    QCOMPARE(branches.size(), 1);

    auto mainBranch = branches.at(0);
    QVERIFY(!mainBranch.isNull());

    auto tree = mainBranch.tree();
    QVERIFY(!tree.isNull());

    auto file = tree.file("README.md");
    QVERIFY(!file.isNull());

    auto content = file.content();
    QCOMPARE(content, mFileContentAtThirdCommit);
}

#include "moc_filetest.cpp"
