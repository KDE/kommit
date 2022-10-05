/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

class TagTest : public QObject
{
    Q_OBJECT
public:
    explicit TagTest(QObject *parent = nullptr);
    ~TagTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
