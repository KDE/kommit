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
class Tree;
class Blob;
class ITree;
}

class DiffWindowPrivate;

class LIBKOMMITWIDGETS_EXPORT DiffWindow : public AppMainWindow
{
    Q_OBJECT

public:
    enum Mode { Files, Dirs };
    explicit DiffWindow();
    DiffWindow(QSharedPointer<Git::Blob> oldFile, QSharedPointer<Git::Blob> newFile);
    DiffWindow(Git::Manager *git, QSharedPointer<Git::ITree> leftTree);
    DiffWindow(Git::Manager *git, QSharedPointer<Git::ITree> leftTree, QSharedPointer<Git::ITree> rightTree);
    DiffWindow(Mode mode, const QString &left, const QString &right);

    void setLeft(const QString &filePath);
    void setLeft(QSharedPointer<Git::Blob> blob);

    void setRight(const QString &filePath);
    void setRight(QSharedPointer<Git::Blob> blob);

    void compare();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void fileOpen();
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewFileSelected(const QString &file);

    DiffWindowPrivate *d_ptr;
    Q_DECLARE_PRIVATE(DiffWindow)
};
