/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "notetest.h"
#include "caches/commitscache.h"
#include "entities/commit.h"
#include "testcommon.h"

#include <QTest>
#include <entities/note.h>
#include <repository.h>

QTEST_GUILESS_MAIN(NoteTest)

NoteTest::NoteTest(QObject *parent)
    : QObject{parent}
    , mNoteText{"Sample note"}
{
}

void NoteTest::initTestCase()
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

void NoteTest::makeACommit()
{
    TestCommon::touch(mManager->path() + "/README.md");

    mManager->addFile("README.md");
    mManager->commit("commit1");
}

void NoteTest::readEmptyNote()
{
    auto commits = mManager->commits()->allCommits();
    QCOMPARE(commits.size(), 1);

    auto commit = commits.first();
    auto note = commit.note();
    QCOMPARE(note, Git::Note{});
}

void NoteTest::addNote()
{
    auto commit = mManager->commits()->allCommits().first();

    auto ok = commit.createNote(mNoteText);
    QVERIFY(ok);
}

void NoteTest::readNote()
{
    auto commit = mManager->commits()->allCommits().first();
    auto note = commit.note();
    QCOMPARE(note.message(), mNoteText);
}

#include "moc_notetest.cpp"
