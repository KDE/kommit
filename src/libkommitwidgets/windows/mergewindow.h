/*
Copyright (C) 2020 Hamed Masafi <hamed.masafi@gmail.com>
SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appmainwindow.h"
#include "ui_kommitmergeview.h"
#include <diff.h>

#include "libkommitwidgets_export.h"

#include <QScopedPointer>

namespace Git
{
class Repository;
class Blob;
}

class SegmentsMapper;
class QLabel;
class MergeWidget;
class MergeWindowPrivate;
class LIBKOMMITWIDGETS_EXPORT MergeWindow : public AppMainWindow
{
    Q_OBJECT
public:
    enum Mode {
        NoParams,
        MergeByParams,
    };

    explicit MergeWindow(Git::Repository *git, Mode mode = NoParams, QWidget *parent = nullptr);
    ~MergeWindow();

    void compare();

    void setLocalFile(const QString &filePath);
    void setLocalFile(const Git::Blob &blob);

    void setRemoteFile(const QString &filePath);
    void setRemoteFile(const Git::Blob &blob);

    void setBaseFile(const QString &filePath);
    void setBaseFile(const Git::Blob &blob);

    void setResultFile(const QString &newFilePathResult);

private Q_SLOTS:
    void fileSave();
    void fileSaveAs();
    void fileOpen();

    void slotSameSizeActivated();

    void slotMergeWidgetConflictsChanged(int conflicts);

private:
protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QScopedPointer<MergeWindowPrivate> d_ptr;
    Q_DECLARE_PRIVATE(MergeWindow)
};
