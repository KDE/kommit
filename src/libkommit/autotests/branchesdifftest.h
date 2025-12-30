/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QTemporaryDir>

namespace Git
{
class Repository;
}

class BranchesDiffTest : public QObject
{
    Q_OBJECT
public:
    explicit BranchesDiffTest(QObject *parent = nullptr);
    ~BranchesDiffTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void makeACommit();
    void createBranch();
    void switchToNewBranch();
    void switchToMaster();
    void diff();

private:
    Git::Repository *mManager;
    QTemporaryDir mDir;
    QString mNewBranchName{"dev"};
};
