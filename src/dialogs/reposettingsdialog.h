/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_reposettingsdialog.h"

class RepoSettingsDialog : public AppDialog, private Ui::RepoSettingsDialog
{
    Q_OBJECT
public:
    explicit RepoSettingsDialog(Git::Manager *git, QWidget *parent = nullptr);

private:
    void slotAccepted();
};
