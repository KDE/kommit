/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_filestreedialog.h"

namespace Git
{
class Repository;
class Tree;
class ITree;
class Tree;
};

class FileActions;
class TreeModel;
class LIBKOMMITWIDGETS_EXPORT FilesTreeDialog : public AppDialog, private Ui::FilesTreeDialog
{
    Q_OBJECT

public:
    FilesTreeDialog(Git::Repository *git, const QString &place, QWidget *parent = nullptr);
    FilesTreeDialog(Git::Repository *git, QSharedPointer<Git::ITree> tree, QWidget *parent = nullptr);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewCustomContextMenuRequested(const QPoint &pos);
    LIBKOMMITWIDGETS_NO_EXPORT void slotListWidgetCustomContextMenuRequested(const QPoint &pos);
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewClicked(const QModelIndex &index);
    LIBKOMMITWIDGETS_NO_EXPORT void slotExtract();
    // LIBKOMMITWIDGETS_NO_EXPORT void initModel(const QStringList &files);
    LIBKOMMITWIDGETS_NO_EXPORT void initModel(QSharedPointer<Git::Tree> tree);

    TreeModel *const mTreeModel;
    const QString mPlace;
    FileActions *const mActions;
    QSharedPointer<Git::Tree> mTree;
    QMenu *const mTreeViewMenu;
    QString mExtractPrefix;
};
