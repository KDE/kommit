/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "overlaytest.h"

#include "../../dolphinplugins/statuscache.h"
#include "filestatus.h"
#include "gittestmanager.h"

#include <QTest>

QTEST_MAIN(OverlayTest)

void OverlayTest::test1()
{
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

    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("added.txt"))), FileStatus::Unmodified);
    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("ignored.txt"))), FileStatus::Ignored);
    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("removed.txt"))), FileStatus::Removed);
    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("changed.txt"))), FileStatus::Modified);
    QCOMPARE(cache.fileStatus(tm.absoluteFilePath(QStringLiteral("untracked.txt"))), FileStatus::Untracked);
}

void OverlayTest::checkRootDir()
{
    StatusCache cache;
    auto status = cache.pathStatus(QStringLiteral("/root"));
    QCOMPARE(status, Git::FileStatus::NoGit);
}

#include "moc_overlaytest.cpp"
