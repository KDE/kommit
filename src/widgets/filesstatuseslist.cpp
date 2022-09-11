/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filesstatuseslist.h"

FilesStatusesList::FilesStatusesList(QWidget *parent)
    : QTreeWidget(parent)
{
    setColumnCount(2);
}

const QList<FileStatus> &FilesStatusesList::files() const
{
    return mFiles;
}

void FilesStatusesList::setFiles(const QList<FileStatus> &newFiles)
{
    mFiles = newFiles;
}

bool FilesStatusesList::showCheckboxes() const
{
    return _showCheckboxes;
}

void FilesStatusesList::setShowCheckboxes(bool newShowCheckboxes)
{
    _showCheckboxes = newShowCheckboxes;
}
