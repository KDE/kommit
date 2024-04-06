/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduletest.h"
#include "qtestcase.h"
#include "testcommon.h"
#include <entities/commit.h>
#include <entities/submodule.h>
#include <entities/tree.h>
#include <options/addsubmoduleoptions.h>

#include <QTest>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(SubmoduleTest)

SubmoduleTest::SubmoduleTest(QObject *parent)
    : QObject{parent}
{
}

void SubmoduleTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;

    QVERIFY(!mManager->isValid());

    auto ok = mManager->init(path);
    QVERIFY(ok);

    TestCommon::initSignature(mManager);
}

void SubmoduleTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void SubmoduleTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit1");
}

void SubmoduleTest::addSubmodule()
{
    Git::AddSubmoduleOptions opts;

    opts.url = "https://github.com/HamedMasafi/Logger.git";
    opts.path = "3rdparty/libgit2";
    TestCommon::makePath(mManager, "3rdparty/libgit2");

    auto ok = mManager->addSubmodule(opts);

    QVERIFY(ok);

    auto submodules = mManager->submodules();
    QCOMPARE(submodules.size(), 1);

    auto newSubmodule = submodules.first();
    auto status = newSubmodule->status();
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::InHead), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::InIndex), true);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::InConfig), true);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::InWd), true);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::IndexAdded), true);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::IndexDeleted), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::IndexModified), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdUninitialized), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdAdded), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdDeleted), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdModified), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdIndexModified), true);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdWdModified), false);
    QCOMPARE(static_cast<bool>(status & Git::Submodule::Status::WdUntracked), false);

    TestCommon::touch(mManager->path() + "/3rdparty/libgit2/sample");
    QCOMPARE(static_cast<bool>(newSubmodule->status() & Git::Submodule::Status::WdUntracked), true);

    qDebug() << status << newSubmodule->status();

    // QCOMPARE(status, newSubmodule->status());
}

void SubmoduleTest::addInSameLocation()
{
    Git::AddSubmoduleOptions opts;

    opts.url = "https://github.com/HamedMasafi/Logger.git";
    opts.path = "3rdparty/libgit2";

    auto ok = mManager->addSubmodule(opts);

    QVERIFY(!ok);

    QCOMPARE(mManager->errorCode(), -4);
    QCOMPARE(mManager->errorClass(), 17);
    QCOMPARE(mManager->errorMessage(), "attempt to add submodule '3rdparty/libgit2' that already exists");
}

void SubmoduleTest::checkExists()
{
    auto submodules = mManager->submodules();
    QCOMPARE(submodules.size(), 1);

    auto submodule = submodules.first();

    QCOMPARE(submodule->name(), "3rdparty/libgit2");
    QCOMPARE(submodule->path(), "3rdparty/libgit2");
    // QCOMPARE(submodule->url(), "https://github.com/HamedMasafi/Logger.git");
}

void SubmoduleTest::lookup()
{
    auto submodule = mManager->submodule("3rdparty/libgit2");

    QVERIFY(submodule != nullptr);
    QCOMPARE(submodule->name(), "3rdparty/libgit2");
    QCOMPARE(submodule->path(), "3rdparty/libgit2");
    QCOMPARE(submodule->url(), "https://github.com/HamedMasafi/Logger.git");
    Git::Submodule::StatusFlags statusFlags = Git::Submodule::Status::WdIndexModified | Git::Submodule::Status::IndexAdded | Git::Submodule::Status::InIndex
        | Git::Submodule::Status::InConfig | Git::Submodule::Status::InWd | Git::Submodule::Status::WdUntracked;
    QCOMPARE(submodule->status(), statusFlags);
    QCOMPARE(submodule->branch(), "");
}

void SubmoduleTest::remove()
{
}

void SubmoduleTest::status()
{
}

#include "moc_submoduletest.cpp"
