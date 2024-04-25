/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "dialogs/appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_changedfilesdialog.h"

class ChangedFilesModel;
class ChangedFileActions;
class LIBKOMMITWIDGETS_EXPORT ChangedFilesDialog : public AppDialog, private Ui::ChangedFilesDialog
{
    Q_OBJECT

public:
    explicit ChangedFilesDialog(Git::Manager *git, QWidget *parent = nullptr);
    ~ChangedFilesDialog() override;

private:
    void slotItemDoubleClicked(const QModelIndex &index);
    void slotCustomContextMenuRequested(const QPoint &pos);
    void slotPushCommit();
    void slotStash();

    void readConfig();
    void writeConfig();

    ChangedFileActions *const mActions;
    ChangedFilesModel *const mModel;
};
