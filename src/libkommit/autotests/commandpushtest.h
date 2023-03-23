/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

class CommandPushTest : public QObject
{
    Q_OBJECT
public:
    explicit CommandPushTest(QObject *parent = nullptr);
    ~CommandPushTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
