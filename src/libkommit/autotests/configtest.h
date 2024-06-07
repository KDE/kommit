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

class ConfigTest : public QObject
{
    Q_OBJECT
public:
    explicit ConfigTest(QObject *parent = nullptr);
    ~ConfigTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void setUserInfo();
    void setGlobalConfig();
    void allConfigs();

private:
    Git::Manager *mManager;
};
