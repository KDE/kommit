/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_modificationsdialog.h"

class ModificationsDialog : public AppDialog, private Ui::ModificationsDialog
{
    Q_OBJECT

public:
    explicit ModificationsDialog(QWidget *parent = nullptr);
};
