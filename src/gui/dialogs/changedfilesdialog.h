/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_changedfilesdialog.h"

class ChangedFileActions;
class ChangedFilesDialog : public AppDialog, private Ui::ChangedFilesDialog
{
    Q_OBJECT

public:
    explicit ChangedFilesDialog(Git::Manager *git, QWidget *parent = nullptr);

private:
    void slotItemDoubleClicked(QListWidgetItem *item);
    void slotCustomContextMenuRequested(const QPoint &pos);
    void slotPushCommit();
    void reload();
    ChangedFileActions *const mActions;
};
