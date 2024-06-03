/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_mergecloseeventdialog.h"

class LIBKOMMITWIDGETS_EXPORT MergeCloseEventDialog : public AppDialog, private Ui::CloseEventDialog
{
    Q_OBJECT

public:
    enum ReturnType {
        LeaveAsIs = QDialog::Rejected,
        MarkAsResolved = QDialog::Accepted,
        DontExit,
    };
    explicit MergeCloseEventDialog(QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotCommandLinkButtonMarkResolvedClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotCommandLinkButtonLeaveAsIsClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotCommandLinkButtonDontExitClicked();
};
