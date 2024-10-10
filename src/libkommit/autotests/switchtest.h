/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

namespace Git
{
class Repository;
}

class SwitchTest : public QObject
{
    Q_OBJECT

public:
    explicit SwitchTest(QObject *parent = nullptr);
    ~SwitchTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void switchToDevBranch();
    void switchToTagV1();
    void switchToMasterBranch();

private:
    Git::Repository *mManager;
};
