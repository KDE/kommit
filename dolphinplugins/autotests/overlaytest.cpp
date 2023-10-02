/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "overlaytest.h"

#include "../../dolphinplugins/statuscache.h"
#include "filestatus.h"
#include "gittestmanager.h"

#include <git2.h>

#include <QTest>

QTEST_MAIN(OverlayTest)

void OverlayTest::initTestCase()
{
    git_libgit2_init();
}

void OverlayTest::test1()
{
    QString s1{"test"};
    QString s2{"test/"};
    QStringRef r1{&s1};
    QStringRef r2{&s2};

    auto ret1 = Impl::removeSlashAtEnd(r1);
    auto ret2 = Impl::removeSlashAtEnd(r2);

    QCOMPARE(ret1, "test");
    QCOMPARE(ret2, "test");

    GitTestManager tm;

    tm.init();
    tm.addToIgnore(QStringLiteral("ignored.txt"));
    tm.touch(QStringLiteral("ignored.txt"));
    tm.touch(QStringLiteral("added.txt"));
    tm.touch(QStringLiteral("changed.txt"));
    tm.touch(QStringLiteral("removed.txt"));
    tm.add();
    tm.commit(QStringLiteral("Initial commit"));
    tm.touch(QStringLiteral("changed.txt"));
    tm.remove(QStringLiteral("removed.txt"));
    tm.touch(QStringLiteral("untracked.txt"));

    StatusCache cache;

    using namespace Git;

    //    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("added.txt"))), FileStatus::Unmodified);
    //    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("ignored.txt"))), FileStatus::Ignored);
    //    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("removed.txt"))), FileStatus::Removed);
    //    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("changed.txt"))), FileStatus::Modified);
    //    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("untracked.txt"))), FileStatus::Untracked);
}

void OverlayTest::checkRootDir()
{
    StatusCache cache;
    auto status = cache.setPath(QStringLiteral("/doc/dev/web/zabtkar/Zabtkar/"));
    QCOMPARE(status, true);

    auto st = cache.status("info");
    //    QCOMPARE(st, KVersionControlPlugin::ItemVersion::LocallyModifiedVersion);
}

void OverlayTest::dirTest()
{
    //    StatusCache cache;
    //    cache.setPath("/doc/src/");

    //    KVersionControlPlugin::ItemVersion status;

    //    status = cache.status("flutter");
    //    QCOMPARE(status, Git::FileStatus::NoGit);

    //    status = cache.status("tensorflow");
    //    QCOMPARE(status, Git::FileStatus::Modified);

    //    status = cache.status("CMakeLists.txt.user");
    //    QCOMPARE(status, Git::FileStatus::Ignored);
}

#include "moc_overlaytest.cpp"
