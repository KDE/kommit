/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_filestreedialog.h"

class FileActions;
class TreeModel;
class FilesTreeDialog : public AppDialog, private Ui::FilesTreeDialog
{
    Q_OBJECT

public:
    explicit FilesTreeDialog(const QString &place, QWidget *parent = nullptr);

private Q_SLOTS:
    void on_treeView_clicked(const QModelIndex &index);
    void on_listWidget_customContextMenuRequested(const QPoint &pos);

private:
    TreeModel *mTreeModel = nullptr;
    QString mPlace;
    FileActions *mActions = nullptr;
};
