/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_fetchdialog.h"

namespace Git
{
class Manager;
class FetchObserver;
}

class FetchDialog : public AppDialog, private Ui::FetchDialog
{
    Q_OBJECT

public:
    explicit FetchDialog(Git::Manager *git, QWidget *parent = nullptr);

    void setBranch(const QString &branch);

private:
    void slotAccept();

    Git::FetchObserver *mObserver;
};
