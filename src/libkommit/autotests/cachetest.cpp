/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "cachetest.h"
#include "caches/branchescache.h"
#include "caches/commitscache.h"
#include "caches/remotescache.h"
#include "caches/tagscache.h"
#include "testcommon.h"

#include <QTest>
#include <entities/tag.h>
#include <repository.h>

QTEST_GUILESS_MAIN(CacheTest)

CacheTest::CacheTest(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Repository{this}}
{
}

void CacheTest::initTestCase()
{
    auto path = TestCommon::getTempPath();

    auto ok = mManager->clone("https://github.com/KDE/kommit.git", path);
    QVERIFY(ok);
    TestCommon::initSignature(mManager);
}

void CacheTest::saveData()
{
    mCommits = mManager->commits()->allCommits();
    mBranches = mManager->branches()->allBranches();
    mTags = mManager->tags()->allTags();
    mRemotes = mManager->remotes()->allRemotes();
}

void CacheTest::switchToInvalidPath()
{
    auto ok = mManager->open("/invalid/path");
    QVERIFY(!ok);
}

void CacheTest::checkBranch_data()
{
}

void CacheTest::checkBranch()
{
    QCOMPARE(mManager->commits()->allCommits().size(), 0);
    QCOMPARE(mManager->branches()->allBranches().size(), 0);
    QCOMPARE(mManager->tags()->allTags().size(), 0);
    QCOMPARE(mManager->remotes()->allRemotes().size(), 0);
}

#include "moc_cachetest.cpp"
