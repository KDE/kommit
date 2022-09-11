/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_filehistorydialog.h"

namespace Git {
class Manager;
class File;
}

class FileHistoryDialog : public AppDialog, private Ui::FileHistoryDialog
{
    Q_OBJECT
    Git::Manager *_git;
    QString _fileName;

public:
    explicit FileHistoryDialog(QWidget *parent = nullptr);
    FileHistoryDialog(Git::Manager *git, const QString &fileName, QWidget *parent = nullptr);
    FileHistoryDialog(Git::Manager *git, const Git::File &file, QWidget *parent = nullptr);

private Q_SLOTS:
    void on_listWidget_itemClicked(QListWidgetItem *item);
};

