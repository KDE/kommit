/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>
#include "ui_settingspagebase.h"
#include "ui_settingspagediff.h"
#include "ui_settingspagegit.h"

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    SettingsManager();
    static SettingsManager *instance();
    void exec(QWidget *parentWidget);


public slots:
    void show();

private:
    void settingsChanged();
    Ui::SettingsPageBase pageBase{};
    Ui::SettingsPageDiff pageDiff{};
    Ui::SettingsPageGit pageGit{};
    QWidget * createBasePage();
    QWidget *createGitPage();
    QWidget *createDiffPage();
};

