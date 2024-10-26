/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_closeeventdialog.h"

class CloseEventDialog : public QDialog, private Ui::CloseEventDialog
{
    Q_OBJECT

public:
    enum ReturnType {
        LeaveAsIs = QDialog::Rejected,
        MarkAsResolved = QDialog::Accepted,
        DontExit
    };
    explicit CloseEventDialog(QWidget *parent = nullptr);
private slots:
    void slotCommandLinkButtonMarkResolvedClicked();
    void slotCommandLinkButtonLeaveAsIsClicked();
    void slotCommandLinkButtonDontExitClicked();
};
