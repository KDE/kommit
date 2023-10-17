/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "entities/file.h"
#include "ui_fileblamedialog.h"

namespace Git
{
class Manager;
}

class FileBlameDialog : public AppDialog, private Ui::FileBlameDialog
{
    Q_OBJECT

public:
    explicit FileBlameDialog(Git::Manager *git, const Git::File &file, QWidget *parent = nullptr);

private:
    void loadData();

    void slotPlainTextEditBlockSelected();

    QString mFileName;
    Git::File mFile;
};
