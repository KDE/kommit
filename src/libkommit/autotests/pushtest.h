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

class PushTest : public QObject
{
    Q_OBJECT
public:
    explicit PushTest(QObject *parent = nullptr);
    ~PushTest() override;

private Q_SLOTS:
    void initTestCase();
    void makeCommit();
    void push();
    void cleanupTestCase();

private:
    Git::Repository *mRepo;
    Git::Repository *mBareRepo;
    QTemporaryDir dir;
    // QTemporaryDir server;
};
