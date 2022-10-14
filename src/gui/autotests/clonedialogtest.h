/*
    SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include <QObject>

class CloneDialogTest : public QObject
{
    Q_OBJECT
public:
    explicit CloneDialogTest(QObject *parent = nullptr);
    ~CloneDialogTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
    void shouldChangePath();
    void shouldChangePath_data();
};
