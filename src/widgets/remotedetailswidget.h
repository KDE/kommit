/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_remotedetailswidget.h"

namespace Git
{
class Manager;
}

// TODO: delete me
class RemoteDetailsWidget : public QWidget, private Ui::RemoteDetailsWidget
{
    Q_OBJECT
    Git::Manager *_git{nullptr};
    QString _branch;

public:
    explicit RemoteDetailsWidget(QWidget *parent = nullptr);
    const QString &branch() const;
    void setBranch(const QString &newBranch);
    Git::Manager *git() const;
    void setGit(Git::Manager *newGit);

private Q_SLOTS:
    void reload();
    //    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
};
