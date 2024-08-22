/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_ignorefiledialog.h"

#include "appdialog.h"
#include "libkommitwidgets_export.h"

class LIBKOMMITWIDGETS_EXPORT IgnoreFileDialog : public AppDialog, private Ui::IgnoreFileDialog
{
    Q_OBJECT

public:
    explicit IgnoreFileDialog(Git::Manager *git, const QString &filePath, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void generateIgnorePattern();
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccept();
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT QString getIgnoreFile() const;
    bool mIsIgnoredAlready{false};
    QString mPath;
    QString mName;
    QString mExt;
};
