/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergecloseeventdialog.h"

MergeCloseEventDialog::MergeCloseEventDialog(QWidget *parent)
    : AppDialog(parent)
{
    setupUi(this);

    connect(commandLinkButtonMarkResolved, &QCommandLinkButton::clicked, this, &MergeCloseEventDialog::slotCommandLinkButtonMarkResolvedClicked);
    connect(commandLinkButtonLeaveAsIs, &QCommandLinkButton::clicked, this, &MergeCloseEventDialog::slotCommandLinkButtonLeaveAsIsClicked);
    connect(commandLinkButtonDontExit, &QCommandLinkButton::clicked, this, &MergeCloseEventDialog::slotCommandLinkButtonDontExitClicked);
}

void MergeCloseEventDialog::slotCommandLinkButtonMarkResolvedClicked()
{
    accept();
}

void MergeCloseEventDialog::slotCommandLinkButtonLeaveAsIsClicked()
{
    reject();
}

void MergeCloseEventDialog::slotCommandLinkButtonDontExitClicked()
{
    done(DontExit);
}

#include "moc_mergecloseeventdialog.cpp"
