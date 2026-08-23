/*
SPDX-FileCopyrightText: 2026 Méven Car <meven@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include <QTemporaryDir>

namespace Git
{
class Repository;
}

class RunnerDialogTest : public QObject
{
    Q_OBJECT

public:
    explicit RunnerDialogTest(QObject *parent = nullptr);
    ~RunnerDialogTest() override = default;

private Q_SLOTS:
    void initTestCase();

    void shouldSurviveBeingClosedWhileGitIsStillRunning();
    void shouldSurviveBeingStoppedWhileGitIsStillRunning();

private:
    QTemporaryDir mDir;
    Git::Repository *mRepository{nullptr};
};
