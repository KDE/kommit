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

private Q_SLOTS:
    void on_pushButtonCommitPush_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    void reload();
    ChangedFileActions *mActions = nullptr;
};
