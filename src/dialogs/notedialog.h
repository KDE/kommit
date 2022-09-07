/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include "ui_notedialog.h"

#include "core/appdialog.h"

class NoteDialog : public AppDialog, private Ui::NoteDialog
{
    Q_OBJECT
    QString _branchName;

public:
    explicit NoteDialog(Git::Manager *git, const QString &branchName, QWidget *parent = nullptr);

private slots:
    void on_buttonBox_accepted();
};

#endif // NOTEDIALOG_H
