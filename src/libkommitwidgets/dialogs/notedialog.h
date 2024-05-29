/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_notedialog.h"

#include "appdialog.h"
#include "libkommitwidgets_export.h"

namespace Git
{
class Commit;
};

class LIBKOMMITWIDGETS_EXPORT NoteDialog : public AppDialog, private Ui::NoteDialog
{
    Q_OBJECT

public:
    explicit NoteDialog(Git::Manager *git, const QString &branchName, QWidget *parent = nullptr);
    explicit NoteDialog(Git::Manager *git, QSharedPointer<Git::Commit> commit, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotAccepted();
    QString mBranchName;
    QSharedPointer<Git::Commit> mCommit;
};
