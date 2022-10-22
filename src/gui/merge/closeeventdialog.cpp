/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "closeeventdialog.h"

CloseEventDialog::CloseEventDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    connect(commandLinkButtonMarkResolved, &QCommandLinkButton::clicked, this, &::CloseEventDialog::slotCommandLinkButtonLeaveAsIsClicked);
    connect(commandLinkButtonLeaveAsIs, &QCommandLinkButton::clicked, this, &CloseEventDialog::slotCommandLinkButtonLeaveAsIsClicked);
    connect(commandLinkButtonDontExit, &QCommandLinkButton::clicked, this, &CloseEventDialog::slotCommandLinkButtonDontExitClicked);
}

void CloseEventDialog::slotCommandLinkButtonMarkResolvedClicked()
{
    accept();
}

void CloseEventDialog::slotCommandLinkButtonLeaveAsIsClicked()
{
    reject();
}

void CloseEventDialog::slotCommandLinkButtonDontExitClicked()
{
    done(DontExit);
}
