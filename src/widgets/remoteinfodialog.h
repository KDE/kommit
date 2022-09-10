/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_remoteinfodialog.h"
namespace Git {
class CommandAddRemote;
}
class RemoteInfoDialog : public QDialog, private Ui::RemoteInfoDialog
{
    Q_OBJECT

public:
    explicit RemoteInfoDialog(QWidget *parent = nullptr);

    QString remoteName() const;
    QString remoteUrl() const;
    Git::CommandAddRemote *command();
};

