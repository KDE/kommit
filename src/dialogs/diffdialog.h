/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_diffdialog.h"
#include <gitfile.h>

class DiffTreeModel;
class DiffDialog : public AppDialog, private Ui::DiffDialog
{
    Q_OBJECT


public:
    Q_DECL_DEPRECATED explicit DiffDialog(QWidget *parent = nullptr);
    Q_DECL_DEPRECATED DiffDialog(const Git::File &oldFile, const Git::File &newFile, QWidget *parent = nullptr);
    Q_DECL_DEPRECATED DiffDialog(const QString &oldBranch, const QString &newBranch, QWidget *parent = nullptr);

private Q_SLOTS:
    void on_toolButtonShowHiddenChars_clicked(bool checked);

    void on_pushButtonSaveAs_clicked();

    void on_treeView_clicked(const QModelIndex &index);

private:
    void compare(const QStringList &oldData, const QStringList &newData);
    Git::File mOldFile;
    Git::File mNewFile;

    QString mOldBranch;
    QString mNewBranch;

    DiffTreeModel *mDiffModel{};
};
