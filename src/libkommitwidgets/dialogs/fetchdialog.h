/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_fetchdialog.h"

#include <Kommit/Oid>
#include <Kommit/Reference>

namespace Git
{
class Repository;
class FetchObserver;
struct PackProgress;
struct FetchTransferStat;
class Credential;
class Certificate;
class Fetch;
}

class LIBKOMMITWIDGETS_EXPORT FetchDialog : public AppDialog, private Ui::FetchDialog
{
    Q_OBJECT

public:
    explicit FetchDialog(Git::Repository *git, QWidget *parent = nullptr);

    void setBranch(const QString &branch);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccept();

    Git::FetchObserver *const mObserver;

    void startFetch();

    void slotFetchMessage(const QString &message);
    void slotFetchTransferProgress(const Git::FetchTransferStat *stat);
    void slotFetchPackProgress(const Git::PackProgress *p);
    void slotFetchUpdateRef(const Git::Reference &reference, const Git::Oid &a, const Git::Oid &b);
    void slotFetchFinished(bool success);

    Git::Fetch *mFetch;
};
