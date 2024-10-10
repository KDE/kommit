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

class FileTest : public QObject
{
    Q_OBJECT
public:
    explicit FileTest(QObject *parent = nullptr);
    ~FileTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void makeFirstCommit();
    void makeSecondCommit();
    void makeThirdCommit();
    void checkContents();
    void checkContentInBranch();

private:
    Git::Repository *mManager;

    QString mFileContentAtFirstCommit;
    QString mFileContentAtSecondCommit;
    QString mFileContentAtThirdCommit;
};
