/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "core/appdialog.h"
#include "ui_filehistorydialog.h"

namespace Git
{
class Manager;
class File;
}

class FileHistoryDialog : public AppDialog, private Ui::FileHistoryDialog
{
    Q_OBJECT

public:
    explicit FileHistoryDialog(Git::Manager *git, const QString &fileName, QWidget *parent = nullptr);
    explicit FileHistoryDialog(Git::Manager *git, const Git::File &file, QWidget *parent = nullptr);

private:
    static constexpr int dataRole{Qt::UserRole + 1};

    void slotListWidgetItemClicked(QListWidgetItem *item);
    void slotTreeViewItemClicked(QTreeWidgetItem *item, int column);
    void slotRadioButtonRegularViewToggled(bool toggle);
    void slotRadioButtonDifferentialViewToggled(bool toggle);
    void compareFiles();

    const QString mFileName;
    QTreeWidgetItem *mLeftFile{nullptr};
    QTreeWidgetItem *mRightFile{nullptr};
};
