/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QTemporaryDir>

namespace Git
{
class Repository;
}

class ChangedFilesTest : public QObject
{
    Q_OBJECT
public:
    explicit ChangedFilesTest(QObject *parent = nullptr);
    ~ChangedFilesTest() override;

private Q_SLOTS:
    void initTestCase();
    void makeACommit();
    void changeSomeFiles();
    void checkChangedFiles();

private:
    QTemporaryDir dir;
    Git::Repository *mManager;
};
