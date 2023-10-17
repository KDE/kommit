/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filetest.h"
#include "entities/branch.h"
#include "entities/tree.h"
#include "gitmanager.h"
#include "testcommon.h"
#include <QTest>

#include <entities/commit.h>

QTEST_GUILESS_MAIN(FileTest)

FileTest::FileTest(QObject *parent)
    : QObject{parent}
{
}

void FileTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    auto init = mManager->init(path);
    QCOMPARE(mManager->path(), path);
    QVERIFY(init);
    QVERIFY(mManager->isValid());
}

void FileTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void FileTest::makeFirstCommit()
{
    mFileContentAtFirstCommit = TestCommon::touch(mManager->path() + "/README.md");
    mManager->addFile("README.md");
    mManager->commit("first commit");
}

void FileTest::makeSecondCommit()
{
    mFileContentAtSecondCommit = TestCommon::touch(mManager->path() + "/README.md");
    mManager->addFile("README.md");
    mManager->commit("second commit");
}

void FileTest::makeThirdCommit()
{
    mFileContentAtThirdCommit = TestCommon::touch(mManager->path() + "/README.md");
    mManager->addFile("README.md");
    mManager->commit("third commit");
}

void FileTest::checkContents()
{
    auto commits = mManager->commits();
    QCOMPARE(commits.size(), 3);
    QCOMPARE(commits.at(2)->subject(), "first commit");
    QCOMPARE(commits.at(1)->subject(), "second commit");
    QCOMPARE(commits.at(0)->subject(), "third commit");

    Git::File firstFile{mManager, commits.at(2)->commitHash(), "README.md"};
    Git::File secondFile{mManager, commits.at(1)->commitHash(), "README.md"};
    Git::File thirdFile{mManager, commits.at(0)->commitHash(), "README.md"};

    QCOMPARE(firstFile.content(), mFileContentAtFirstCommit);
    QCOMPARE(secondFile.content(), mFileContentAtSecondCommit);
    QCOMPARE(thirdFile.content(), mFileContentAtThirdCommit);

    QCOMPARE(commits.at(2)->tree()->file("README.md")->content(), mFileContentAtFirstCommit);
    QCOMPARE(commits.at(1)->tree()->file("README.md")->content(), mFileContentAtSecondCommit);
    QCOMPARE(commits.at(0)->tree()->file("README.md")->content(), mFileContentAtThirdCommit);
}

void FileTest::checkContentInBranch()
{
    auto branches = mManager->branches(Git::Manager::BranchType::LocalBranch);
    QCOMPARE(branches.size(), 1);

    auto mainBranch = branches.at(0);
    QVERIFY(mainBranch != nullptr);

    auto tree = mainBranch->tree();
    QVERIFY(tree != nullptr);

    auto file = tree->file("README.md");
    QVERIFY(file != nullptr);

    auto content = file->content();
    QCOMPARE(content, mFileContentAtThirdCommit);
}

#include "moc_filetest.cpp"
