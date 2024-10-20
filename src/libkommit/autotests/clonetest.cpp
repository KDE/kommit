/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clonetest.h"
#include "caches/commitscache.h"
#include "caches/tagscache.h"
#include "testcommon.h"
#include <Kommit/Commit>
#include <entities/tree.h>

#include <QTest>
#include <repository.h>

QTEST_GUILESS_MAIN(CloneTest)

CloneTest::CloneTest(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Repository{this}}
{
}

CloneTest::~CloneTest()
{
    delete mManager;
}

void CloneTest::initTestCase()
{
    QVERIFY(!mManager->isValid());
}

void CloneTest::clone()
{
    auto path = TestCommon::getTempPath(false);
    qDebug() << path;
    auto ok = mManager->clone("https://invent.kde.org/sdk/kommit.git", path);
    qDebug() << mManager->path();
    QVERIFY(ok);

    TestCommon::initSignature(mManager);

    path.append("/");
    QCOMPARE(path, mManager->path());

    auto tags = mManager->tags()->allNames();
    QVERIFY(tags.contains("v1.0.1"));
}

void CloneTest::initialCommitTree()
{
    auto commit = mManager->commits()->find("05659b9f92b7932bb2c04ced181dbdde294cb0bb");
    qDebug() << commit.message();
    QVERIFY(!commit.isNull());

    auto tree = commit.tree();

    auto topLevelItems = tree.entries("");

    QCOMPARE(topLevelItems.type("snapcraft.yaml"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type("Messages.sh"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type("LICENSE"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type("COPYING.DOC"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type("COPYING"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type("CMakeLists.txt"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type(".gitignore"), Git::Tree::EntryType::File);
    QCOMPARE(topLevelItems.type("README.md"), Git::Tree::EntryType::File);

    QCOMPARE(topLevelItems.type("src"), Git::Tree::EntryType::Dir);
    QCOMPARE(topLevelItems.type("icons"), Git::Tree::EntryType::Dir);
    QCOMPARE(topLevelItems.type("dolphinplugins"), Git::Tree::EntryType::Dir);
    QCOMPARE(topLevelItems.type("doc"), Git::Tree::EntryType::Dir);
    QCOMPARE(topLevelItems.type("autotests"), Git::Tree::EntryType::Dir);

    auto src = tree.entries("src", Git::Tree::EntryType::Dir);
    qDebug() << src;

    QVERIFY(src.contains("widgets"));
    QVERIFY(src.contains("settings"));
    QVERIFY(src.contains("models"));
    QVERIFY(src.contains("merge"));
    QVERIFY(src.contains("git"));
    QVERIFY(src.contains("diff"));
    QVERIFY(src.contains("dialogs"));
    QVERIFY(src.contains("core"));
    QVERIFY(src.contains("actions"));
}

void CloneTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

#include "moc_clonetest.cpp"
