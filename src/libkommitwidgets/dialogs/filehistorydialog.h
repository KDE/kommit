/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_filehistorydialog.h"

namespace Git
{
class Manager;
class File;
}

class LIBKOMMITWIDGETS_EXPORT FileHistoryDialog : public AppDialog, private Ui::FileHistoryDialog
{
    Q_OBJECT

public:
    explicit FileHistoryDialog(Git::Manager *git, const QString &fileName, QWidget *parent = nullptr);
    explicit FileHistoryDialog(Git::Manager *git, QSharedPointer<Git::Blob> file, QWidget *parent = nullptr);

private:
    static constexpr int dataRole{Qt::UserRole + 1};

    LIBKOMMITWIDGETS_NO_EXPORT void slotListWidgetItemClicked(QListWidgetItem *item);
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewItemClicked(QTreeWidgetItem *item, int column);
    LIBKOMMITWIDGETS_NO_EXPORT void slotRadioButtonRegularViewToggled(bool toggle);
    LIBKOMMITWIDGETS_NO_EXPORT void slotRadioButtonDifferentialViewToggled(bool toggle);
    LIBKOMMITWIDGETS_NO_EXPORT void compareFiles();

    const QString mFileName;
    QTreeWidgetItem *mLeftFile{nullptr};
    QTreeWidgetItem *mRightFile{nullptr};
};
