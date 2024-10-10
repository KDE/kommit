/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

namespace Git
{
class Remote;
class Tag;
class Branch;
class Commit;
class Repository;
};

class CacheTest : public QObject
{
    Q_OBJECT
public:
    explicit CacheTest(QObject *parent = nullptr);
    ~CacheTest() override = default;

private Q_SLOTS:
    void initTestCase();

    void saveData();
    void switchToInvalidPath();
    void checkBranch_data();
    void checkBranch();

private:
    Git::Repository *mManager;
    QList<QSharedPointer<Git::Commit>> mCommits;
    QList<QSharedPointer<Git::Branch>> mBranches;
    QList<QSharedPointer<Git::Tag>> mTags;
    QList<QSharedPointer<Git::Remote>> mRemotes;
};
