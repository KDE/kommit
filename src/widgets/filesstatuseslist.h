/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "git/filestatus.h"
#include <QTreeWidget>

class FilesStatusesList : public QTreeWidget
{
    Q_OBJECT
public:
    explicit FilesStatusesList(QWidget *parent = nullptr);

    const QList<FileStatus> &files() const;
    void setFiles(const QList<FileStatus> &newFiles);
    bool showCheckboxes() const;
    void setShowCheckboxes(bool newShowCheckboxes);

private:
    QList<FileStatus> mFiles;
    bool mShowCheckboxes{false};
};
