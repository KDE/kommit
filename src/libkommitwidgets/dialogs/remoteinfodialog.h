/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_remoteinfodialog.h"
namespace Git
{
class CommandAddRemote;
}
class RemoteInfoDialog : public QDialog, private Ui::RemoteInfoDialog
{
    Q_OBJECT

public:
    explicit RemoteInfoDialog(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT QString remoteName() const;
    Q_REQUIRED_RESULT QString remoteUrl() const;
    Q_REQUIRED_RESULT Git::CommandAddRemote *command();
};
