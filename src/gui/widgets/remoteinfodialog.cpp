/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remoteinfodialog.h"
#include "commands/commandaddremote.h"
#include <QPushButton>

RemoteInfoDialog::RemoteInfoDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);

    checkBoxTags->setCheckState(Qt::PartiallyChecked);
    auto button = buttonBox->button(QDialogButtonBox::Ok);
    connect(lineEditName, &QLineEdit::textChanged, this, [button](const QString &str) {
        button->setEnabled(!str.trimmed().isEmpty());
    });
    // Disable as lineEditName is empty
    button->setEnabled(false);
}

QString RemoteInfoDialog::remoteName() const
{
    return lineEditName->text();
}

QString RemoteInfoDialog::remoteUrl() const
{
    return lineEditUrl->text();
}

Git::CommandAddRemote *RemoteInfoDialog::command()
{
    auto cmd = new Git::CommandAddRemote(this);
    cmd->setTags(checkBoxTags->checkState());
    cmd->setRemoteName(lineEditName->text());
    cmd->setUrl(lineEditUrl->text());
    cmd->setMirror(checkBoxMirror->isChecked());
    cmd->setFetch(checkBoxFetch->isChecked());
    return cmd;
}
