/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_taginfodialog.h"

class LIBKOMMITWIDGETS_EXPORT TagInfoDialog : public AppDialog, private Ui::TagInfoDialog
{
    Q_OBJECT

public:
    explicit TagInfoDialog(QWidget *parent = nullptr);
    [[nodiscard]] QString tagName() const;
    void setTagName(const QString &newTagName);
    [[nodiscard]] QString message() const;
    void setMessage(const QString &newMessage);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotLineEditTagNameTextChanged(const QString &s);
    LIBKOMMITWIDGETS_NO_EXPORT void updateOkButton();
    QPushButton *mOkButton = nullptr;
};
