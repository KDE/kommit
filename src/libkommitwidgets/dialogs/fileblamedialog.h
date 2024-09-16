/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_fileblamedialog.h"

namespace Git
{
class Manager;
class Blob;
}

class LIBKOMMITWIDGETS_EXPORT FileBlameDialog : public AppDialog, private Ui::FileBlameDialog
{
    Q_OBJECT

public:
    explicit FileBlameDialog(Git::Manager *git, QSharedPointer<Git::Blob> file, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void loadData();

    LIBKOMMITWIDGETS_NO_EXPORT void slotPlainTextEditBlockSelected();

    QString mFileName;
    QSharedPointer<Git::Blob> const mFile;
};
