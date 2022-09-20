/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_pulldialog.h"

namespace Git
{
class Manager;
}
class PullDialog : public AppDialog, private Ui::PullDialog
{
    Q_OBJECT

public:
    explicit PullDialog(QWidget *parent = nullptr, Git::Manager *git = nullptr);

private Q_SLOTS:
    void on_buttonBox_accepted();
};
