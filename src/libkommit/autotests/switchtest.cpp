/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "switchtest.h"

#include "testcommon.h"

#include <QTest>
#include <Kommit/BranchesCache>
#include <Kommit/TagsCache>
#include <Kommit/Blob>
#include <Kommit/File>
#include <Kommit/Index>
#include <Kommit/Tag>
#include <Kommit/Repository>

QTEST_GUILESS_MAIN(SwitchTest)

SwitchTest::SwitchTest(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Repository}
{
}

void SwitchTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;

    QVERIFY(TestCommon::extractSampleRepo(path));

    auto ok = mManager->open(path);
    QVERIFY(ok);
    TestCommon::initSignature(mManager);
}

void SwitchTest::switchToMasterBranch()
{
    auto masterBranch = mManager->branches()->findByName("master");
    QVERIFY(!masterBranch.isNull());

    QVERIFY(mManager->switchBranch(masterBranch));
    QCOMPARE(mManager->branches()->currentName(), "master");
    auto content = TestCommon::readFile(mManager->path() + "/content.txt");
    QCOMPARE(content, "file3 created\n");
}

void SwitchTest::switchToDevBranch()
{
    auto devBranch = mManager->branches()->findByName("dev");
    QVERIFY(!devBranch.isNull());
    QVERIFY(mManager->switchBranch(devBranch));

    auto content = TestCommon::readFile(mManager->path() + "/content.txt");
    auto contentFile = mManager->headTree()->file("content.txt");
    QCOMPARE(content, "file2 edited\n");
    QCOMPARE(contentFile->content(), content);
}

void SwitchTest::switchToTagV1()
{
    auto tag = mManager->tags()->find("v1.0");
    QVERIFY(!tag.isNull());

    QVERIFY(mManager->reset(tag->commit(), Git::Repository::ResetType::Hard));

    auto content = TestCommon::readFile(mManager->path() + "/content.txt");
    QCOMPARE(content, "file1 created\n");
}

#include "moc_switchtest.cpp"
