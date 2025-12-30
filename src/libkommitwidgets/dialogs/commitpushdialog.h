/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_commitpushdialog.h"

#include <Kommit/Certificate>
#include <Kommit/PushOptions>

namespace Git
{
class Repository;
class Credential;
}
class ChangedFileActions;
class ChangedFilesModel;
class LIBKOMMITWIDGETS_EXPORT CommitPushDialog : public AppDialog, private Ui::CommitPushDialog
{
    Q_OBJECT

public:
    explicit CommitPushDialog(Git::Repository *git, QWidget *parent = nullptr);
    ~CommitPushDialog() override;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushMessage(const QString &message);
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushTransferProgress(const Git::FetchTransferStat *stat);
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushPackProgress(const Git::PackProgress *p);
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushUpdateRef(const Git::Reference &reference, const Git::Oid &a, const Git::Oid &b);
    Q_INVOKABLE LIBKOMMITWIDGETS_NO_EXPORT void slotFetchFinished(bool success);
    LIBKOMMITWIDGETS_NO_EXPORT void slotCredentialRequested(const QString &url, Git::Credential *cred, bool *accept);
    LIBKOMMITWIDGETS_NO_EXPORT void slotCertificateCheck(const Git::Certificate &cert, bool *accept);

    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonCommitClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonPushClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotToolButtonAddAllClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotToolButtonAddNoneClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotToolButtonAddIndexedClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotToolButtonAddAddedClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotToolButtonAddRemovedClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotToolButtonAddModifiedClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotListWidgetItemDoubleClicked(const QModelIndex &index);
    LIBKOMMITWIDGETS_NO_EXPORT void slotGroupBoxMakeCommitToggled(bool);
    LIBKOMMITWIDGETS_NO_EXPORT void slotListWidgetCustomContextMenuRequested(const QPoint &pos);
    LIBKOMMITWIDGETS_NO_EXPORT void checkButtonsEnable();
    LIBKOMMITWIDGETS_NO_EXPORT void addFiles();
    LIBKOMMITWIDGETS_NO_EXPORT void commit();
    LIBKOMMITWIDGETS_NO_EXPORT void reload();
    LIBKOMMITWIDGETS_NO_EXPORT void readConfig();
    LIBKOMMITWIDGETS_NO_EXPORT void writeConfig();

    int mRetryCount{};
    bool mIsChanged{};
    Git::PushOptions mPushOptions;
    ChangedFileActions *mActions = nullptr;
    ChangedFilesModel *const mChangedFilesModel;
};
