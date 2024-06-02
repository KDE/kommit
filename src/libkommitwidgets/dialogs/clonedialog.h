/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_clonedialog.h"

#include "commands/commandclone.h"

namespace Git
{

class Credential;
class CloneObserver;
}

class LIBKOMMITWIDGETS_EXPORT CloneDialog : public AppDialog, private Ui::CloneDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(QWidget *parent = nullptr);
    ~CloneDialog() override;

    Q_REQUIRED_RESULT Git::CloneCommand *command();

    void setLocalPath(const QString &path);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotCredentialRequeted(const QString &url, Git::Credential *cred);
    LIBKOMMITWIDGETS_NO_EXPORT void slotUrlChanged(const QString &text);
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccepted();
    LIBKOMMITWIDGETS_NO_EXPORT void loadSettings();
    QString mFixedPath;

    Git::CloneObserver *const mCloneObserver;
};
