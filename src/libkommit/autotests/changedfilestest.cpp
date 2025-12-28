/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfilestest.h"
#include "caches/branchescache.h"
#include "caches/commitscache.h"
#include "entities/blob.h"
#include "entities/branch.h"
#include "entities/file.h"
#include "entities/index.h"
#include "entities/tree.h"
#include "repository.h"
#include "testcommon.h"
#include <QTest>

#include <Kommit/CommitsCache>
#include <QRandomGenerator>
#include <entities/commit.h>

QTEST_GUILESS_MAIN(ChangedFilesTest)

Git::StatusFlags statusFromPorcelain(const QString &xy)
{
    if (xy.size() < 2)
        return Git::StatusFlag::Current;

    const QChar x = xy.at(0);
    const QChar y = xy.at(1);

    Git::StatusFlags status = Git::StatusFlag::Current;

    // --- conflicts ---
    static const QStringList conflicts = {"UU", "AA", "DD", "AU", "UA", "DU", "UD"};
    if (conflicts.contains(xy))
        return Git::StatusFlag::Conflicted;

    // --- index (X) ---
    switch (x.unicode()) {
    case 'A':
        status |= Git::StatusFlag::IndexNew;
        break;
    case 'M':
        status |= Git::StatusFlag::IndexModified;
        break;
    case 'D':
        status |= Git::StatusFlag::IndexDeleted;
        break;
    case 'R':
        status |= Git::StatusFlag::IndexRenamed;
        break;
    case 'T':
        status |= Git::StatusFlag::IndexTypechange;
        break;
    default:
        break;
    }

    // --- working tree (Y) ---
    switch (y.unicode()) {
    case '?':
        status |= Git::StatusFlag::WtNew;
        break;
    case 'M':
        status |= Git::StatusFlag::WtModified;
        break;
    case 'D':
        status |= Git::StatusFlag::WtDeleted;
        break;
    case 'R':
        status |= Git::StatusFlag::WtRenamed;
        break;
    case 'T':
        status |= Git::StatusFlag::WtTypechange;
        break;
    case '!':
        status |= Git::StatusFlag::Ignored;
        break;
    default:
        break;
    }

    return status;
}

ChangedFilesTest::ChangedFilesTest(QObject *parent)
    : QObject{parent}
{
}

ChangedFilesTest::~ChangedFilesTest()
{
    delete mManager;
}

void ChangedFilesTest::initTestCase()
{
    auto path = dir.path();
    qDebug() << path;
    mManager = new Git::Repository;
    QVERIFY(mManager->init(path));
    QVERIFY(mManager->isValid());

    TestCommon::initSignature(mManager);
}

void ChangedFilesTest::makeACommit()
{
    auto index = mManager->index();
    for (auto i = 0; i < 20; ++i) {
        auto file = QStringLiteral("file_%1.txt").arg(i);
        TestCommon::touch(mManager, file);
        index.addByPath(file);
    }
    QVERIFY(index.writeTree());
    QVERIFY(mManager->commit("test"));

    auto allCommits = mManager->commits()->allCommits();
    QCOMPARE(allCommits.size(), 1);
    QCOMPARE(allCommits[0].message(), "test");
}

void ChangedFilesTest::changeSomeFiles()
{
    auto index = mManager->index();

    for (auto i = 0; i < 20; ++i) {
        auto file = QStringLiteral("file_%1.txt").arg(i);
        auto n = QRandomGenerator::global()->bounded(0, 3);

        switch (n) {
        case 0:
            continue;
            break;
        case 1:
            TestCommon::touch(mManager, file);
            break;
        case 2:
            TestCommon::touch(mManager, file);
            QVERIFY(index.addByPath(file));
            break;
        }
    }

    QVERIFY(index.writeTree());
}

void ChangedFilesTest::checkChangedFiles()
{
    auto statusOutput = TestCommon::runGit(dir.path(), {"--no-pager", "status", "--short", "--porcelain"});
    auto lines = statusOutput.split('\n');

    QMap<QString, Git::StatusFlags> filesMap;
    for (auto &line : lines) {
        if (line.trimmed().isEmpty())
            continue;
        filesMap.insert(line.mid(3), statusFromPorcelain(line.mid(0, 2)));
    }

    auto changedFiles = mManager->changedFiles();

    for (auto i = changedFiles.begin(); i != changedFiles.end(); ++i) {
        QCOMPARE(i.value(), filesMap[i.key()]);
    }
}

#include "moc_changedfilestest.cpp"
