/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "indextest.h"
#include "gitmanager.h"
#include "testcommon.h"
#include <QTest>

QTEST_GUILESS_MAIN(IndexTest)

IndexTest::IndexTest(QObject *parent)
    : QObject{parent}
{
}

void IndexTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    auto init = mManager->init(path);
    QCOMPARE(mManager->path(), path);
    QVERIFY(init);
    QVERIFY(mManager->isValid());
}

void IndexTest::cleanupTestCase()
{
    //    TestCommon::cleanPath(mManager);
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

    TestCommon::touch(mManager->path() + "/README.md");
    auto changedFiles = mManager->changedFiles();
    QVERIFY(!changedFiles.contains("README.md"));
}

#include "moc_indextest.cpp"
