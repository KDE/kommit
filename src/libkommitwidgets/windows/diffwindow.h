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
    DiffWindow(Git::Manager *git, Git::Tag *tag);
    DiffWindow(Git::Branch *oldBranch, Git::Branch *newBranch);
    DiffWindow(Git::Manager *git, QSharedPointer<Git::Tree> leftTree);
    DiffWindow(const QString &oldDir, const QString &newDir);

private Q_SLOTS:
    void fileOpen();
    void slotTreeViewFileSelected(const QString &file);

private:
    Git::Manager *mGit;
    QSharedPointer<Git::File> mOldFile;
    QSharedPointer<Git::File> mNewFile;

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

    struct Storage {
        enum class Mode { NoStorage, FileSystem, Git, Tree };

        QSharedPointer<Git::File> file(const QString &path) const;

        void setGitManager(Git::Manager *manager);
        void setPath(const QString &path);
        void setTree(QSharedPointer<Git::Tree> tree);

    private:
        QString mPath;
        QSharedPointer<Git::Tree> mTree;
        Git::Manager *mManager;
        Mode mMode{Mode::NoStorage};
    };

    Storage mLeftStorage;
    Storage mRightStorage;
    void compareDirs();
};
