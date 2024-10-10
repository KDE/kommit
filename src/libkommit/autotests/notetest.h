/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

namespace Git
{
class Repository;
};

class NoteTest : public QObject
{
    Q_OBJECT
public:
    explicit NoteTest(QObject *parent = nullptr);
    ~NoteTest() override = default;

private Q_SLOTS:
    void initTestCase();

    void makeACommit();
    void readEmptyNote();
    void addNote();
    void readNote();

private:
    QString mNoteText;
    Git::Repository *mManager;
};
