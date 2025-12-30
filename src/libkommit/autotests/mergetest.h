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

class MergeTest : public QObject
{
    Q_OBJECT
public:
    explicit MergeTest(QObject *parent = nullptr);
    ~MergeTest() override = default;

private Q_SLOTS:
    void initTestCase();
    void mergeDev();
    void mergeChanges();

private:
    Git::Repository *mManager;
    QTemporaryDir mDir;
};
