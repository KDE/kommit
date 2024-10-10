/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_initdialog.h"

class LIBKOMMITWIDGETS_EXPORT InitDialog : public AppDialog, private Ui::InitDialog
{
    Q_OBJECT

public:
    explicit InitDialog(Git::Repository *git, QWidget *parent = nullptr);
    [[nodiscard]] QString path() const;
    void setPath(const QString &path);
};
