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
#include <Kommit/PushOptions>
#include <Kommit/RemoteCallbacks>
#include <Kommit/RemotesCache>
#include <Kommit/Repository>
#include <Kommit/Tree>

#include <QTest>

QTEST_GUILESS_MAIN(PushTest)

PushTest::PushTest(QObject *parent)
    : QObject{parent}
    , mLocalRepo{new Git::Repository{this}}
    , mBareRepo{new Git::Repository{this}}
{
}

PushTest::~PushTest()
{
    delete mLocalRepo;
}

void PushTest::initTestCase()
{
    Git::InitOptions options;
    options.setFlags(Git::InitOptions::Bare);
    QVERIFY(mBareRepo->init(mBareDir.path(), &options));

    QVERIFY(mLocalRepo->init(mLocalDir.path()));
    QVERIFY(mLocalRepo->isValid());
    QVERIFY(mLocalRepo->remotes()->create("origin", mBareDir.path()));
}

void PushTest::makeCommit()
{
    TestCommon::touch(mLocalRepo, "file1.txt");
    auto index = mLocalRepo->index();
    index.addByPath("file1.txt");
    index.writeTree();
    QVERIFY(mLocalRepo->commits()->create("commit1"));
}

void PushTest::push()
{
    auto branch = mLocalRepo->branches()->findByName("master");
    auto remote = mLocalRepo->remotes()->findByName("origin");

    QVERIFY(!branch.isNull());
    QVERIFY(!remote.isNull());

    auto options = new Git::PushOptions(mLocalRepo);

    connect(options->remoteCallbacks(), &Git::RemoteCallbacks::credentialRequested, this, &PushTest::credentialRequested, Qt::BlockingQueuedConnection);

    QVERIFY(mLocalRepo->push(branch, remote, options));
    QVERIFY(!mIsCredentialRequested);

    auto originBranch = mLocalRepo->branches()->findByName("origin/master");
    QVERIFY(!originBranch.isNull());
    QVERIFY(branch.isHead());
    QCOMPARE(branch.commit().commitHash(), originBranch.commit().commitHash());
}

void PushTest::checkCommit()
{
    QTemporaryDir dir;
    auto manager = new Git::Repository;

    QVERIFY(manager->clone(mBareDir.path(), dir.path()));
    QCOMPARE(manager->remotes()->allNames().size(), 1);
    QCOMPARE(manager->commits()->allCommits().size(), 1);
}

void PushTest::credentialRequested(const QString &url, Git::Credential *cred, bool *accept)
{
    Q_UNUSED(cred)
    qDebug() << url;
    *accept = true;
    mIsCredentialRequested = true;
}

#include "moc_pushtest.cpp"
