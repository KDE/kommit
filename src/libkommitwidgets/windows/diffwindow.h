/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appmainwindow.h"
#include "libkommitwidgets_export.h"

#include <entities/file.h>

#include <QScopedPointer>
#include <QSharedPointer>

#include <Kommit/Blob>
#include <Kommit/ITree>

namespace Git
{
class Repository;
}

class DiffWindowPrivate;

class LIBKOMMITWIDGETS_EXPORT DiffWindow : public AppMainWindow
{
    Q_OBJECT

public:
    enum Mode {
        Files,
        Dirs
    };
    explicit DiffWindow();
    DiffWindow(const Git::Blob &oldFile, const Git::Blob &newFile);
    DiffWindow(Git::Repository *git, Git::ITree *leftTree);
    DiffWindow(Git::Repository *git, Git::ITree *leftTree, Git::ITree *rightTree);
    DiffWindow(Mode mode, const QString &left, const QString &right);
    ~DiffWindow();

    void setLeft(const QString &filePath);
    void setLeft(Git::Blob blob);

    void setRight(const QString &filePath);
    void setRight(Git::Blob blob);

    void compare();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void fileOpen();
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewFileSelected(const QString &file);

    QScopedPointer<DiffWindowPrivate> d_ptr;
    Q_DECLARE_PRIVATE(DiffWindow)
};
