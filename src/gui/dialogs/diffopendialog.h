/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_diffopendialog.h"

class DiffOpenDialog : public AppDialog, private Ui::DiffOpenDialog
{
    Q_OBJECT

public:
    enum Mode { Files, Dirs };
    explicit DiffOpenDialog(QWidget *parent = nullptr);
    ~DiffOpenDialog() override;

    Q_REQUIRED_RESULT QString oldFile() const;
    Q_REQUIRED_RESULT QString newFile() const;
    Q_REQUIRED_RESULT QString oldDir() const;
    Q_REQUIRED_RESULT QString newDir() const;
    Q_REQUIRED_RESULT Mode mode() const;

private:
    void readSettings();
    void saveSettings();
};
