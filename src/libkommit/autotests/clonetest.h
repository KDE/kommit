/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

namespace Git
{
class Manager;
};

class CloneTest : public QObject
{
    Q_OBJECT
public:
    explicit CloneTest(QObject *parent = nullptr);
    ~CloneTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void clone();
    void initialCommitTree();
    void cleanupTestCase();

private:
    Git::Manager *mManager;
};
