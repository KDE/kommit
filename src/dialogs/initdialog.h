/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_initdialog.h"

#include "core/appdialog.h"

class InitDialog : public AppDialog, private Ui::InitDialog
{
    Q_OBJECT

public:
    explicit InitDialog(Git::Manager *git, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    Q_REQUIRED_RESULT QString path() const;
    void setPath(const QString &path);
};
