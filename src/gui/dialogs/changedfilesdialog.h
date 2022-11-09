/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_changedfilesdialog.h"

class ChangedFilesModel;
class ChangedFileActions;
class ChangedFilesDialog : public AppDialog, private Ui::ChangedFilesDialog
{
    Q_OBJECT

public:
    explicit ChangedFilesDialog(Git::Manager *git, QWidget *parent = nullptr);

private:
    void slotItemDoubleClicked(const QModelIndex &index);
    void slotCustomContextMenuRequested(const QPoint &pos);
    void slotPushCommit();
    void slotButtonBoxClicked(QAbstractButton *button);

    ChangedFileActions *const mActions;
    ChangedFilesModel *const mModel;
};
