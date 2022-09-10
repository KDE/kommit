/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_fileblamedialog.h"
#include "git/gitfile.h"

namespace Git {
class Manager;
}

class FileBlameDialog : public AppDialog, private Ui::FileBlameDialog
{
    Q_OBJECT
    Git::Manager *_git;
    QString _fileName;
    Git::File _file;

public:
    explicit FileBlameDialog(Git::Manager *git, const QString &fileName, QWidget *parent = nullptr);
    explicit FileBlameDialog(const Git::File &file, QWidget *parent = nullptr);

private slots:
    void on_plainTextEdit_blockSelected();
};

