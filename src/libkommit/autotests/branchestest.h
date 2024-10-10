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

class BranchesTest : public QObject
{
    Q_OBJECT
public:
    explicit BranchesTest(QObject *parent = nullptr);
    ~BranchesTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void makeACommit();
    void createBranch();
    void switchToNewBranch();
    void removeCurrentBranch();
    void switchToMaster();
    void removeNewBranch();

private:
    Git::Repository *mManager;

    QString newBranchName{"new_branch_name"};
};
