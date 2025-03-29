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

class CommitTest : public QObject
{
    Q_OBJECT
public:
    explicit CommitTest(QObject *parent = nullptr);
    ~CommitTest() override;

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void firstCommit();
    void secondCommit();
    void thirdCommit();

    void checkFileContents();

    void amendCommit();
    void checkAmendFileContent();

private:
    Git::Repository *mManager;

    QString mFileContent1;
    QString mFileContent2;
    QString mFileContent3;
    QString mFileContentAmend;
};
