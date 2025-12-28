/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "pushtest.h"
#include "testcommon.h"
#include <Kommit/BranchesCache>
#include <Kommit/Commit>
#include <Kommit/CommitsCache>
#include <Kommit/Error>
#include <Kommit/Index>
#include <Kommit/RemotesCache>
#include <entities/tree.h>

#include <QTest>
#include <repository.h>

QTEST_GUILESS_MAIN(PushTest)

PushTest::PushTest(QObject *parent)
    : QObject{parent}
    , mRepo{new Git::Repository{this}}
    , mBareRepo{new Git::Repository{this}}
{
}

PushTest::~PushTest()
{
    delete mRepo;
}

void PushTest::initTestCase()
{
    QVERIFY(dir.isValid());
    QVERIFY(mRepo->init(dir.path()));
    Git::InitOptions options;
    options.setFlags(Git::InitOptions::Bare);
    QVERIFY(mBareRepo->init(dir.path(), &options));

    qDebug() << Git::Error::lastTypeString() << Git::Error::lastMessage();
    qDebug() << dir.path();
    QVERIFY(mRepo->isValid());

    // QVERIFY(mRepo->remotes()->create("origin", "file://" + dir.path()));
    QVERIFY(mBareRepo->remotes()->create("origin", "file://" + dir.path()));
}

void PushTest::makeCommit()
{
    TestCommon::touch(mRepo, "file1.txt");
    auto index = mRepo->index();
    index.addByPath("file1.txt");
    index.write();
    QVERIFY(mRepo->commits()->create("commit1"));
}

void PushTest::push()
{
    auto branch = mRepo->branches()->findByName("master");
    auto remote = mRepo->remotes()->findByName("origin");

    QVERIFY(!branch.isNull());
    QVERIFY(!remote.isNull());

    mRepo->push(branch, remote);
}

void PushTest::cleanupTestCase()
{
    TestCommon::cleanPath(mRepo);
}

#include "moc_pushtest.cpp"
