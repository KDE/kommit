/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergetest.h"
#include "caches/stashescache.h"
#include "entities/index.h"
#include "entities/stash.h"
#include "repository.h"
#include "testcommon.h"

#include <Kommit/CommitsCache>
#include <Kommit/BranchesCache>
#include <Kommit/CheckoutOptions>
#include <Kommit/MergeOptions>
#include <QTest>

QTEST_GUILESS_MAIN(MergeTest)

MergeTest::MergeTest(QObject *parent)
    : QObject{parent}
{
}

void MergeTest::initTestCase()
{
    QString path = mDir.path();
    TestCommon::extractSampleRepo("kommit-git-sample-test", path);
    mManager = new Git::Repository{path};
    QVERIFY(mManager->isValid());

    TestCommon::initSignature(mManager);
}

void MergeTest::mergeDev()
{
    auto devBranch = mManager->branches()->findByName("dev");
    QVERIFY(!devBranch.isNull());

    Git::CheckoutOptions checkoutOptions;
    Git::MergeOptions mergeOptions;

    auto changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.size(), 0);

    mManager->merge(devBranch, checkoutOptions, mergeOptions);

    changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.size(), 3);

    QCOMPARE(changedFiles["logindialog.ui"], Git::StatusFlag::IndexModified);
    QCOMPARE(changedFiles["mainwindow.cpp"], Git::StatusFlag::IndexModified);
    QCOMPARE(changedFiles["mainwindow.h"], Git::StatusFlag::IndexModified);
}

void MergeTest::mergeChanges()
{
    auto changeBranch = mManager->branches()->findByName("wip/change");
    QVERIFY(!changeBranch.isNull());

    Git::CheckoutOptions checkoutOptions;
    Git::MergeOptions mergeOptions;

    auto ok = mManager->merge(changeBranch, checkoutOptions, mergeOptions);
    QVERIFY(!ok); // there is uncommitted changes

    auto index = mManager->index();
    index.addAll();
    QVERIFY(mManager->commits()->create("commit merge"));

    ok = mManager->merge(changeBranch, checkoutOptions, mergeOptions);
    QVERIFY(ok); // there is NOT uncommitted changes

    auto changedFiles = mManager->changedFiles();
    QCOMPARE(changedFiles.size(), 3);

    QCOMPARE(changedFiles["mainwindow.cpp"], Git::StatusFlag::IndexModified);
    QCOMPARE(changedFiles["mainwindow.ui"], Git::StatusFlag::IndexModified);
    QCOMPARE(changedFiles["mainwindow.h"], Git::StatusFlag::IndexModified);
}

#include "moc_mergetest.cpp"
