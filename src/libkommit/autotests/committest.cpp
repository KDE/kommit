/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "committest.h"
#include "testcommon.h"

#include <Kommit/Commit>
#include <Kommit/CommitsCache>
#include <Kommit/Index>
#include <Kommit/Repository>
#include <Kommit/TagsCache>
#include <Kommit/Tree>

#include <QTest>

QTEST_GUILESS_MAIN(CommitTest)

CommitTest::CommitTest(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Repository{this}}
{
}

CommitTest::~CommitTest()
{
    delete mManager;
}

void CommitTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    auto init = mManager->init(path);
    QCOMPARE(mManager->path(), path);
    QVERIFY(init);
    QVERIFY(mManager->isValid());

    TestCommon::initSignature(mManager);
}

void CommitTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void CommitTest::firstCommit()
{
    mFileContent1 = TestCommon::touch(mManager, "file.txt");

    auto index = mManager->index();

    QVERIFY(!index.isNull());
    index.addByPath("file.txt");
    index.write();

    auto commitsCount = mManager->commits()->size();
    auto ok = mManager->commits()->create("first commit");
    QVERIFY(ok);
    QCOMPARE(mManager->commits()->size(), commitsCount + 1);
}

void CommitTest::secondCommit()
{
    mFileContent2 = TestCommon::touch(mManager, "file.txt");

    auto index = mManager->index();

    QVERIFY(!index.isNull());
    index.addByPath("file.txt");
    index.write();

    auto commitsCount = mManager->commits()->size();
    auto ok = mManager->commits()->create("second commit");
    QVERIFY(ok);
    QCOMPARE(mManager->commits()->size(), commitsCount + 1);
}

void CommitTest::thirdCommit()
{
    mFileContent3 = TestCommon::touch(mManager, "file.txt");

    auto index = mManager->index();

    QVERIFY(!index.isNull());
    index.addByPath("file.txt");
    index.write();

    auto commitsCount = mManager->commits()->size();
    auto ok = mManager->commits()->create("third commit");
    QVERIFY(ok);
    QCOMPARE(mManager->commits()->size(), commitsCount + 1);
}

void CommitTest::amendCommit()
{
    mFileContentAmend = TestCommon::touch(mManager, "file.txt");

    auto index = mManager->index();

    QVERIFY(!index.isNull());
    index.addByPath("file.txt");
    index.write();

    auto commitsCount = mManager->commits()->size();
    auto ok = mManager->commits()->amend("amend commit");
    QVERIFY(ok);
    QCOMPARE(mManager->commits()->size(), commitsCount);
}

void CommitTest::checkAmendFileContent()
{
    auto commits = mManager->commits()->allCommits();

    QCOMPARE(commits.size(), 3);

    auto commit = mManager->commits()->allCommits().at(0);
    QCOMPARE(commit.message(), "amend commit");
    QCOMPARE(commit.tree().file("file.txt").content(), mFileContentAmend.toUtf8());
}

void CommitTest::checkFileContents()
{
    auto commits = mManager->commits()->allCommits();

    QCOMPARE(commits.size(), 3);
    QCOMPARE(commits[2].message(), "first commit");
    QCOMPARE(commits[1].message(), "second commit");
    QCOMPARE(commits[0].message(), "third commit");

    QCOMPARE(commits[2].tree().file("file.txt").content(), mFileContent1.toUtf8());
    QCOMPARE(commits[1].tree().file("file.txt").content(), mFileContent2.toUtf8());
    QCOMPARE(commits[0].tree().file("file.txt").content(), mFileContent3.toUtf8());
}

#include "moc_committest.cpp"
