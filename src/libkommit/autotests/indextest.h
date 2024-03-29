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

class IndexTest : public QObject
{
    Q_OBJECT
public:
    explicit IndexTest(QObject *parent = nullptr);
    ~IndexTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void addFile();
    void revertFile();
    void removeFile();

private:
    Git::Manager *mManager;
};
