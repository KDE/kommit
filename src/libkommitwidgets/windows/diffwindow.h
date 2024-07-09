/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appmainwindow.h"
#include "libkommitwidgets_export.h"

#include <entities/file.h>

#include <QSharedPointer>

namespace Git
{
class Manager;
class Tag;
class Tree;
class Branch;
};

class DiffTreeModel;
class DiffWidget;
class DiffTreeView;
class FilesModel;
class LIBKOMMITWIDGETS_EXPORT DiffWindow : public AppMainWindow
{
    Q_OBJECT

public:
    explicit DiffWindow();
    explicit DiffWindow(Git::Manager *git);
    DiffWindow(QSharedPointer<Git::File> oldFile, QSharedPointer<Git::File> newFile);
    DiffWindow(Git::Manager *git, const QString &oldBranch, const QString &newBranch);
    DiffWindow(Git::Manager *git, QSharedPointer<Git::Tag> tag);
    DiffWindow(Git::Branch *oldBranch, Git::Branch *newBranch);
    DiffWindow(QSharedPointer<Git::Branch> oldBranch, QSharedPointer<Git::Branch> newBranch);
    DiffWindow(Git::Manager *git, QSharedPointer<Git::Tree> leftTree);
    DiffWindow(const QString &oldDir, const QString &newDir);

private:
    LIBKOMMITWIDGETS_NO_EXPORT void fileOpen();
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewFileSelected(const QString &file);
    LIBKOMMITWIDGETS_NO_EXPORT void compareDirs();
    QSharedPointer<Git::File> mOldFile;
    QSharedPointer<Git::File> mNewFile;

    QString mOldBranch;
    QString mNewBranch;

    QString mLeftDir;
    QString mRightDir;

    FilesModel *mFilesModel = nullptr;
    DiffTreeModel *mDiffModel = nullptr;
    DiffWidget *mDiffWidget = nullptr;
    DiffTreeView *mTreeView = nullptr;
    QDockWidget *mDock = nullptr;

    LIBKOMMITWIDGETS_NO_EXPORT void initActions();
    LIBKOMMITWIDGETS_NO_EXPORT void init(bool showSideBar);

    enum Mode { None, Dirs, Files };

    struct Storage {
        enum class Mode { NoStorage, FileSystem, Git, Tree };

        QSharedPointer<Git::File> file(const QString &path) const;

        LIBKOMMITWIDGETS_NO_EXPORT void setGitBranch(Git::Manager *manager, const QString &branchName);
        LIBKOMMITWIDGETS_NO_EXPORT void setPath(const QString &path);
        LIBKOMMITWIDGETS_NO_EXPORT void setTree(QSharedPointer<Git::Tree> tree);

    private:
        QString mPath;
        QString mBranchName;
        QSharedPointer<Git::Tree> mTree;
        Git::Manager *mManager;
        Mode mMode{Mode::NoStorage};
    };

    Storage mLeftStorage;
    Storage mRightStorage;
};
