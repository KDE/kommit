/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_diffopendialog.h"

class DiffOpenDialog : public AppDialog, private Ui::DiffOpenDialog
{
    Q_OBJECT

public:
    enum Mode { Files, Dirs };
    explicit DiffOpenDialog(QWidget *parent = nullptr);
    ~DiffOpenDialog() override;

    QString oldFile() const;
    QString newFile() const;
    QString oldDir() const;
    QString newDir() const;
    Mode mode() const;

private Q_SLOTS:
    void on_toolButtonBrowseNewFile_clicked();
    void on_toolButtonBrowseOldFile_clicked();
    void on_toolButtonBrowseNewDirectory_clicked();
    void on_toolButtonBrowseOldDirectory_clicked();
};
