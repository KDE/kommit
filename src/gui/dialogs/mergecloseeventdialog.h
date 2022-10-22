/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_mergecloseeventdialog.h"

class MergeCloseEventDialog : public AppDialog, private Ui::CloseEventDialog
{
    Q_OBJECT

public:
    enum ReturnType { LeaveAsIs = QDialog::Rejected, MarkAsResolved = QDialog::Accepted, DontExit };
    explicit MergeCloseEventDialog(QWidget *parent = nullptr);
private Q_SLOTS:
    void slotCommandLinkButtonMarkResolvedClicked();
    void slotCommandLinkButtonLeaveAsIsClicked();
    void slotCommandLinkButtonDontExitClicked();
};
