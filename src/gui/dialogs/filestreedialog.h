/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_filestreedialog.h"

namespace Git
{
class Manager;
};

class FileActions;
class TreeModel;
class FilesTreeDialog : public AppDialog, private Ui::FilesTreeDialog
{
    Q_OBJECT

public:
    explicit FilesTreeDialog(Git::Manager *git, const QString &place, QWidget *parent = nullptr);

private Q_SLOTS:
    void slotTreeViewClicked(const QModelIndex &index);
    void slotListWidgetCustomContextMenuRequested(const QPoint &pos);

private:
    TreeModel *const mTreeModel;
    const QString mPlace;
    FileActions *const mActions;
};
