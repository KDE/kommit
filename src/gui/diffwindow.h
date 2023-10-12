/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appmainwindow.h"
#include <entities/file.h>

namespace Git
{
class Manager;
};

class DiffTreeModel;
class DiffWidget;
class DiffTreeView;
class FilesModel;
class DiffWindow : public AppMainWindow
{
    Q_OBJECT

public:
    explicit DiffWindow();
    explicit DiffWindow(Git::Manager *git);
    DiffWindow(const Git::File &oldFile, const Git::File &newFile);
    DiffWindow(Git::Manager *git, const QString &oldBranch, const QString &newBranch);
    DiffWindow(const QString &oldDir, const QString &newDir);

private Q_SLOTS:
    void fileOpen();
    void slotTreeViewFileSelected(const QString &file);

private:
    Git::Manager *mGit;
    Git::File mOldFile;
    Git::File mNewFile;

    QString mOldBranch;
    QString mNewBranch;

    QString mLeftDir, mRightDir;

    FilesModel *mFilesModel;
    DiffTreeModel *mDiffModel;
    DiffWidget *mDiffWidget;
    DiffTreeView *mTreeView;
    QDockWidget *mDock;

    void initActions();
    void init(bool showSideBar);

    enum Mode { None, Dirs, Files };
    enum Storage { NoStorage, FileSystem, Git };

    Storage mLeftStorage{NoStorage};
    Storage mRightStorage{NoStorage};
    void compareDirs();
};
