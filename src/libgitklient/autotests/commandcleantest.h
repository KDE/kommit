/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include <QObject>

class CommandCleanTest : public QObject
{
    Q_OBJECT
public:
    explicit CommandCleanTest(QObject *parent = nullptr);
    ~CommandCleanTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
