/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_diffopendialog.h"

class LIBKOMMITWIDGETS_EXPORT DiffOpenDialog : public AppDialog, private Ui::DiffOpenDialog
{
    Q_OBJECT

public:
    enum Mode {
        Files,
        Dirs,
    };
    explicit DiffOpenDialog(QWidget *parent = nullptr);
    ~DiffOpenDialog() override;

    [[nodiscard]] QString oldFile() const;
    [[nodiscard]] QString newFile() const;
    [[nodiscard]] QString oldDir() const;
    [[nodiscard]] QString newDir() const;
    [[nodiscard]] Mode mode() const;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void readSettings();
    LIBKOMMITWIDGETS_NO_EXPORT void saveSettings();
    LIBKOMMITWIDGETS_NO_EXPORT void updateOkButton();
};
