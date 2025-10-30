/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagtest.h"
#include "caches/branchescache.h"
#include "caches/tagscache.h"
#include "testcommon.h"

#include <QTest>
#include <entities/tag.h>
#include <repository.h>
#include <Kommit/CommitsCache>

QTEST_GUILESS_MAIN(TagTest)

TagTest::TagTest(QObject *parent)
    : QObject{parent}
{
}

void TagTest::initTestCase()
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

void TagTest::shouldHaveDefaultValues()
{
    Git::Tag w;
    QVERIFY(w.message().isEmpty());
    QVERIFY(w.name().isEmpty());
    QVERIFY(w.tagger().isNull());
}

void TagTest::addTagNoHead()
{
    auto ok = mManager->tags()->create("tag1", "sample message");

    QVERIFY(!ok);

    auto tags = mManager->tags()->allNames();
    QVERIFY(!tags.contains("tag1"));
}

void TagTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    auto ok = mManager->commits()->create("commit1");
    QVERIFY(ok);
}

void TagTest::addTag()
{
    auto ok = mManager->tags()->create("tag1", "sample message");

    QVERIFY(ok);

    auto tags = mManager->tags()->allNames();
    QVERIFY(tags.contains("tag1"));
}

void TagTest::removeTag()
{
    auto tag = mManager->tags()->find("tag1");
    // auto object = dynamic_cast<Git::Object *>(&tag);

    // QVERIFY(!object->isNull());
    // QCOMPARE(object->type(), Git::Object::Type::Tag);

    auto ok = mManager->tags()->remove(tag);

    QVERIFY(ok);

    auto tags = mManager->tags()->allNames();
    QVERIFY(!tags.contains("tag1"));
}

#include "moc_tagtest.cpp"
