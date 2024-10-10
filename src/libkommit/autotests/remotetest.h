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

class RemoteTest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteTest(QObject *parent = nullptr);
    ~RemoteTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void addRemote();
    void fetch();
    void renameRemote();
    void removeRemote();
    void pull();

private:
    Git::Repository *mManager;
};
