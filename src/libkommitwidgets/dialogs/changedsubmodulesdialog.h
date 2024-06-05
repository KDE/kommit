/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_changedsubmodulesdialog.h"

class ChangedFilesModel;
class LIBKOMMITWIDGETS_EXPORT ChangedSubmodulesDialog : public AppDialog, private Ui::ChangedSubmodulesDialog
{
    Q_OBJECT

public:
    explicit ChangedSubmodulesDialog(Git::Manager *git, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotComitPushButtonClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void reload();
    ChangedFilesModel *mModel = nullptr;
    QMap<QPushButton *, QString> mButtonsMap;
};
