/*
    SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

class FilesStatusesListTest : public QObject
{
    Q_OBJECT
public:
    explicit FilesStatusesListTest(QObject *parent = nullptr);
    ~FilesStatusesListTest() override = default;
private Q_SLOTS:
    void shouldHaveDefaultValues();
};
