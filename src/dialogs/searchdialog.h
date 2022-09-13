/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "../core/appdialog.h"
#include "ui_searchdialog.h"

namespace Git
{
class Manager;
}

class QStandardItemModel;
class SearchDialog : public AppDialog, private Ui::SearchDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(const QString &path, Git::Manager *git, QWidget *parent = nullptr);
    explicit SearchDialog(Git::Manager *git, QWidget *parent = nullptr);

    void initModel();

public Q_SLOTS:
    void on_pushButtonSearch_clicked();
    void on_treeView_doubleClicked(QModelIndex index);

private:
    void beginSearch();
    void searchOnPlace(const QString &place, const QString &commit);

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    Git::Manager *mGit = nullptr;

    struct {
        int value;
        int total;
        QString message;
    } _progress;
    QStandardItemModel *mModel{nullptr};
};
