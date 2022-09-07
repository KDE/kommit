/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "closeeventdialog.h"

CloseEventDialog::CloseEventDialog(QWidget *parent) :
      QDialog(parent)
{
    setupUi(this);
}

void CloseEventDialog::on_commandLinkButtonMarkResolved_clicked()
{
    accept();
}


void CloseEventDialog::on_commandLinkButtonLeaveAsIs_clicked()
{
    reject();
}


void CloseEventDialog::on_commandLinkButtonDontExit_clicked()
{
    done(DontExit);
}

