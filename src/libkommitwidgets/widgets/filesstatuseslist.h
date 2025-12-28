/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "filestatus.h"
#include "libkommitwidgets_export.h"
#include <QTreeWidget>

// TODO: mark for delete
class LIBKOMMITWIDGETS_EXPORT FilesStatusesList : public QTreeWidget
{
    Q_OBJECT
public:
    explicit FilesStatusesList(QWidget *parent = nullptr);

    const QList<Git::FileStatus> &files() const;
    void setFiles(const QList<Git::FileStatus> &newFiles);
    [[nodiscard]] bool showCheckboxes() const;
    void setShowCheckboxes(bool newShowCheckboxes);

private:
    QList<Git::FileStatus> mFiles;
    bool mShowCheckboxes{false};
};
