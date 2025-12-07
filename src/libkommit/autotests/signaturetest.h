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

class SignatureTest : public QObject
{
    Q_OBJECT

public:
    explicit SignatureTest(QObject *parent = nullptr);
    ~SignatureTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void emptysign();
    void defaultSign();
    void nowSign();

private:
    Git::Repository *mManager;
};
