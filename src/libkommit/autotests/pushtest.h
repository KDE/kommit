/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QProcess>
#include <QTemporaryDir>

namespace Git
{
class Repository;
class Credential ;
}

class PushTest : public QObject
{
    Q_OBJECT
public:
    explicit PushTest(QObject *parent = nullptr);
    ~PushTest() override;

private Q_SLOTS:
    void initTestCase();
    void makeCommit();
    void push();
    void checkCommit();

private:
    void credentialRequested(const QString &url, Git::Credential *cred, bool *accept);
    Git::Repository *mLocalRepo;
    Git::Repository *mBareRepo;
    QTemporaryDir mBareDir;
    QTemporaryDir mLocalDir;
    QProcess mGitDaemon;
    bool mIsCredentialRequested{false};
};
