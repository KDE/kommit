/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_fileblamedialog.h"

#include <Kommit/Blame>

namespace Git
{
class Repository;
class Blob;
}

class LIBKOMMITWIDGETS_EXPORT FileBlameDialog : public AppDialog, private Ui::FileBlameDialog
{
    Q_OBJECT

public:
    explicit FileBlameDialog(Git::Repository *git, const QString &file, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void loadData();

    LIBKOMMITWIDGETS_NO_EXPORT void slotPlainTextEditBlockSelected();

    QString mFileName;
    QString mFile;
    Git::Blame mBlameData;
};
