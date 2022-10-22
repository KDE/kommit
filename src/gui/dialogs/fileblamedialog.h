/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "gitfile.h"
#include "ui_fileblamedialog.h"

namespace Git
{
class Manager;
}

class FileBlameDialog : public AppDialog, private Ui::FileBlameDialog
{
    Q_OBJECT

public:
    explicit FileBlameDialog(Git::Manager *git, const QString &fileName, QWidget *parent = nullptr);
    explicit FileBlameDialog(const Git::File &file, QWidget *parent = nullptr);

private Q_SLOTS:
    void slotPlainTextEditBlockSelected();

private:
    QString mFileName;
    Git::File mFile;
};
