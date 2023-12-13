/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_cleanupdialog.h"

namespace Git
{
class CommandClean;
}
class LIBKOMMITWIDGETS_EXPORT CleanupDialog : public AppDialog, private Ui::CleanupDialog
{
    Q_OBJECT

public:
    explicit CleanupDialog(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT Git::CommandClean *command() const;
};
