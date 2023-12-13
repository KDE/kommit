/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_clonedialog.h"

#include "commands/commandclone.h"

class LIBKOMMITWIDGETS_EXPORT CloneDialog : public AppDialog, private Ui::CloneDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(QWidget *parent = nullptr);
    ~CloneDialog() override;

    Q_REQUIRED_RESULT Git::CloneCommand *command();

    void setLocalPath(const QString &path);

private:
    void slotUrlChanged(const QString &text);
    void slotAccepted();
    void loadSettings();
    QString mFixedPath;
};
