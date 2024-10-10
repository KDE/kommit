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

class TagTest : public QObject
{
    Q_OBJECT
public:
    explicit TagTest(QObject *parent = nullptr);
    ~TagTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void shouldHaveDefaultValues();

    void addTagNoHead();
    void makeACommit();
    void addTag();
    void removeTag();

private:
    Git::Repository *mManager;
};
