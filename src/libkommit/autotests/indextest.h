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

class IndexTest : public QObject
{
    Q_OBJECT
public:
    explicit IndexTest(QObject *parent = nullptr);
    ~IndexTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void addFile();
    void revertFile();
    void removeFile();
    void revertFileOfFour();

private:
    Git::Repository *mManager;
};
