/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_branchesselectionwidget.h"

#include "libkommitwidgets_export.h"

namespace Git
{
class Manager;
class Branch;
};

class BranchesSelectionWidgetPrivate;
class LIBKOMMITWIDGETS_EXPORT BranchesSelectionWidget : public QWidget, private Ui::BranchesSelectionWidget
{
    Q_OBJECT

public:
    explicit BranchesSelectionWidget(QWidget *parent = nullptr);

    Q_REQUIRED_RESULT Git::Manager *git() const;
    void setGit(Git::Manager *git);

    void reload();

Q_SIGNALS:
    void branchActivated(QSharedPointer<Git::Branch> branch);

private:
    void slotTreeViewRepoItemActivated(const QModelIndex &index);

    BranchesSelectionWidgetPrivate *d_ptr;
    Q_DECLARE_PRIVATE(BranchesSelectionWidget)
};
