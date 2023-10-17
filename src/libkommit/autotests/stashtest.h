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

class StashTest : public QObject
{
    Q_OBJECT
public:
    explicit StashTest(QObject *parent = nullptr);
    ~StashTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void makeACommit();
    void touchAFile();
    void makeStash();
    void applyStash();

private:
    Git::Manager *mManager;
};
