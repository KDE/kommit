/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef CLEANUPDIALOG_H
#define CLEANUPDIALOG_H

#include "core/appdialog.h"
#include "ui_cleanupdialog.h"

namespace Git
{
class CommandClean;
}
class CleanupDialog : public AppDialog, private Ui::CleanupDialog
{
    Q_OBJECT

public:
    explicit CleanupDialog(QWidget *parent = nullptr);

    Git::CommandClean *command() const;
};

#endif // CLEANUPDIALOG_H
