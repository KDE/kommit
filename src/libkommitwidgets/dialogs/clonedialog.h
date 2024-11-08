/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_clonedialog.h"

#include "commands/commandclone.h"

#include <Kommit/CheckoutOptions>
#include <Kommit/FileDelta>

namespace Git
{
class CloneOptions;
class Credential;
}

class LIBKOMMITWIDGETS_EXPORT CloneDialog : public AppDialog, private Ui::CloneDialog
{
    Q_OBJECT

public:
    explicit CloneDialog(Git::Repository *git, QWidget *parent = nullptr);
    ~CloneDialog() override;

    [[nodiscard]] Git::CloneCommand *command();

    void setLocalPath(const QString &path);

private:
    void slotCheckoutNotify(Git::CheckoutOptions::NotifyFlag notify, QString path, Git::DiffFile baseline, Git::DiffFile target, Git::DiffFile workdir);
    void slotCheckoutProgress(QString path, size_t completed_steps, size_t total_steps);
    void slotCheckoutPerfData(size_t mkdir_calls, size_t stat_calls, size_t chmod_calls);

    LIBKOMMITWIDGETS_NO_EXPORT void performClone();
    LIBKOMMITWIDGETS_NO_EXPORT void slotCredentialRequeted(const QString &url, Git::Credential *cred);
    LIBKOMMITWIDGETS_NO_EXPORT void slotUrlChanged(const QString &text);
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccepted();
    LIBKOMMITWIDGETS_NO_EXPORT void loadSettings();
    LIBKOMMITWIDGETS_NO_EXPORT void updateOkButton();
    QString mFixedPath;

    Git::CloneOptions *mCloneOptions;
};
