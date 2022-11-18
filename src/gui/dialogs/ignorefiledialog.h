/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_ignorefiledialog.h"

#include "core/appdialog.h"

class IgnoreFileDialog : public AppDialog, private Ui::IgnoreFileDialog
{
    Q_OBJECT

public:
    explicit IgnoreFileDialog(Git::Manager *git, const QString &filePath, QWidget *parent = nullptr);

private:
    void generateIgnorePattern();
    void slotAccept();
    QString getIgnoreFile() const;
    bool mIsIgnoredAlready{false};
    QString mPath;
    QString mName;
    QString mExt;
};
