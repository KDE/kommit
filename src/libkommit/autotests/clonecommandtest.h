/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

class CloneCommandTest : public QObject
{
    Q_OBJECT
public:
    explicit CloneCommandTest(QObject *parent = nullptr);
    ~CloneCommandTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldGenerateCommand_data();
    void shouldGenerateCommand();
};
