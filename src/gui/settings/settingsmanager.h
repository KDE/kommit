/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_settingspagebase.h"
#include "ui_settingspagediff.h"
#include "ui_settingspagegit.h"
#include <QObject>

namespace Git
{
class Manager;
};

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    SettingsManager(Git::Manager *git, QWidget *parentWidget);

    void exec(QWidget *parentWidget);
    static void applyToLib();

public Q_SLOTS:
    void show();

Q_SIGNALS:
    void settingsUpdated();

private:
    Git::Manager *const mGit;
    void settingsChanged();
    Ui::SettingsPageBase pageBase{};
    Ui::SettingsPageDiff pageDiff{};
    Ui::SettingsPageGit pageGit{};
    QWidget *createBasePage();
    QWidget *createGitPage();
    QWidget *createDiffPage();
    QWidget *const mParentWidget;
};
