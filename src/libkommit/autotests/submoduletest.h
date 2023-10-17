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

class SubmoduleTest : public QObject
{
    Q_OBJECT
public:
    explicit SubmoduleTest(QObject *parent = nullptr);
    ~SubmoduleTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void makeACommit();

    void addSubmodule();
    void addInSameLocation();
    void checkExists();
    void lookup();
    void remove();

private:
    Git::Manager *mManager;
};
