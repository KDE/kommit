/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_pulldialog.h"
#include <Kommit/CheckoutOptions>
#include <Kommit/FetchOptions>
#include <Kommit/MergeOptions>

namespace Git
{
class Repository;
}
class LIBKOMMITWIDGETS_EXPORT PullDialog : public AppDialog, private Ui::PullDialog
{
    Q_OBJECT

public:
    explicit PullDialog(Git::Repository *git, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccepted();

    Git::FetchOptions mFetchOptions;
    Git::MergeOptions mMergeOptions;
    Git::CheckoutOptions mCheckoutOptions;
};
