/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "indextest.h"
#include "repository.h"
#include "testcommon.h"
#include <QTest>
#include <entities/blob.h>
#include <entities/index.h>

QTEST_GUILESS_MAIN(IndexTest)

IndexTest::IndexTest(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Repository{this}}
{
}

IndexTest::~IndexTest()
{
    delete mManager;
}

void IndexTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    auto init = mManager->init(path);
    QCOMPARE(mManager->path(), path);
    QVERIFY(init);
    QVERIFY(mManager->isValid());

    TestCommon::initSignature(mManager);
}

void IndexTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void IndexTest::addFile()
{
    TestCommon::touch(mManager->path() + "/README.md");

    auto changedFiles = mManager->changedFiles();
    QVERIFY(changedFiles.contains("README.md"));

    mManager->addFile("README.md");

    changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.value("README.md"), Git::ChangeStatus::Added);

    mManager->commit("sample commit");

    changedFiles = mManager->changedFiles();
    QVERIFY(!changedFiles.contains("README.md"));
}

void IndexTest::revertFile()
{
    TestCommon::touch(mManager->path() + "/README.md");

    auto changedFiles = mManager->changedFiles();
    QVERIFY(changedFiles.contains("README.md"));

    auto ok = mManager->revertFile("README.md");
    QVERIFY(ok);

    changedFiles = mManager->changedFiles();
    QVERIFY(!changedFiles.contains("README.md"));
}

void IndexTest::removeFile()
{
    auto ok = mManager->removeFile("README.md", false);
    QVERIFY(ok);

    auto index = mManager->index();
    auto tree = index.tree();
    TestCommon::touch(mManager->path() + "/README.md");
    auto changedFiles = mManager->changedFiles();
    // QVERIFY(!changedFiles.contains("README.md"));
}

void IndexTest::revertFileOfFour()
{
    auto fileOriginalContent1 = TestCommon::touch(mManager->path() + "/1.txt");
    auto fileOriginalContent2 = TestCommon::touch(mManager->path() + "/2.txt");
    auto fileOriginalContent3 = TestCommon::touch(mManager->path() + "/3.txt");
    auto fileOriginalContent4 = TestCommon::touch(mManager->path() + "/4.txt");
    {
        auto index = mManager->index();
        index.addByPath("1.txt");
        index.addByPath("2.txt");
        index.addByPath("3.txt");
        index.addByPath("4.txt");

        index.write();
    }
    mManager->commit("change 4 files");

    auto fileTouchedContent1 = TestCommon::touch(mManager->path() + "/1.txt");
    auto fileTouchedContent2 = TestCommon::touch(mManager->path() + "/2.txt");
    auto fileTouchedContent3 = TestCommon::touch(mManager->path() + "/3.txt");
    auto fileTouchedContent4 = TestCommon::touch(mManager->path() + "/4.txt");

    QVERIFY(mManager->revertFile("1.txt"));

    auto fileContent1 = TestCommon::readFile(mManager->path() + "/1.txt");
    auto fileContent2 = TestCommon::readFile(mManager->path() + "/2.txt");
    auto fileContent3 = TestCommon::readFile(mManager->path() + "/3.txt");
    auto fileContent4 = TestCommon::readFile(mManager->path() + "/4.txt");

    auto changedfiles = mManager->changedFiles();

    QString s{"1.txt"};
    auto ch1 = s.toUtf8();
    auto constCh1 = ch1.constData();
    auto ch2 = s.toStdString();
    auto ch2Str = ch2.c_str();
    QCOMPARE(constCh1, ch2Str);

    QCOMPARE(fileContent1, fileOriginalContent1);
    QCOMPARE(fileContent2, fileTouchedContent2);
    QCOMPARE(fileContent3, fileTouchedContent3);
    QCOMPARE(fileContent4, fileTouchedContent4);
}

void IndexTest::shouldGiveAnIndexThatWorksAfterTheRepositoryWasReadAgain()
{
    const QString name{QStringLiteral("after-reading-again.txt")};
    QVERIFY(TestCommon::writeFile(mManager, name, QStringLiteral("one\n")));
    mManager->addFile(name);

    const auto before = mManager->index().blobByPath(name);
    QVERIFY(!before.isNull());

    // The handle the index was read through is closed and opened again in here, and libgit2
    // takes the owner off an index when its repository goes, so one kept from before answers
    // about a repository that is not there any more.
    mManager->reload();

    const auto after = mManager->index().blobByPath(name);
    QVERIFY(!after.isNull());
    QCOMPARE(after.content(), before.content());
}

#include "moc_indextest.cpp"
