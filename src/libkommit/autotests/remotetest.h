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

class RemoteTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteTest(QObject *parent = nullptr);
    ~RemoteTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void addRemote();
    void fetch();
    void renameRemote();
    void removeRemote();
    void pull();

private:
    Git::Manager *mManager;
};
