/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagtest.h"
#include "testcommon.h"

#include <QTest>
#include <entities/tag.h>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(TagTest)

TagTest::TagTest(QObject *parent)
    : QObject{parent}
{
}

void TagTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    auto init = mManager->init(path);
    QCOMPARE(mManager->path(), path);
    QVERIFY(init);
    QVERIFY(mManager->isValid());

    TestCommon::initSignature(mManager);
}

void TagTest::shouldHaveDefaultValues()
{
    Git::Tag w;
    QVERIFY(w.message().isEmpty());
    QVERIFY(w.name().isEmpty());
    QVERIFY(w.tagger().isNull());
}

void TagTest::addTagNoHead()
{
    auto ok = mManager->createTag("tag1", "sample message");

    QVERIFY(!ok);

    auto tags = mManager->tagsNames();
    QVERIFY(!tags.contains("tag1"));
}

void TagTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit1");
}

void TagTest::addTag()
{
    auto ok = mManager->createTag("tag1", "sample message");

    QVERIFY(ok);

    auto tags = mManager->tagsNames();
    QVERIFY(tags.contains("tag1"));
}

void TagTest::removeTag()
{
    auto ok = mManager->removeTag("tag1");

    QVERIFY(ok);

    auto tags = mManager->tagsNames();
    QVERIFY(!tags.contains("tag1"));
}

#include "moc_tagtest.cpp"
