/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "switchtest.h"

#include "testcommon.h"

#include <Kommit/Blob>
#include <Kommit/BranchesCache>
#include <Kommit/File>
#include <Kommit/Index>
#include <Kommit/Repository>
#include <Kommit/Tag>
#include <Kommit/TagsCache>
#include <QTest>

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
    auto content = TestCommon::readFile(mManager->path() + "/shared.txt");
    QCOMPARE(content, "v2 on master\n");
}

void SwitchTest::switchToDevBranch()
{
    auto devBranch = mManager->branches()->findByName("dev");
    QVERIFY(!devBranch.isNull());
    QVERIFY(mManager->switchBranch(devBranch));

    auto content = TestCommon::readFile(mManager->path() + "/shared.txt");

    auto headTree = mManager->headTree();
    QVERIFY(!headTree.isNull());
    auto contentFile = headTree.file("shared.txt");
    QCOMPARE(content, "v2 on dev\n");
    QCOMPARE(contentFile.content(), content);
}

void SwitchTest::switchToTagV1()
{
    auto tag = mManager->tags()->find("v1");
    QVERIFY(!tag.isNull());

    QVERIFY(mManager->reset(tag.commit(), Git::Repository::ResetType::Hard));

    auto content = TestCommon::readFile(mManager->path() + "/v1.txt");
    QCOMPARE(content, "v1 tag\n");
}

#include "moc_switchtest.cpp"
