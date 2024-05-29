/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_searchdialog.h"

namespace Git
{
class Manager;
}

class QStandardItemModel;
class LIBKOMMITWIDGETS_EXPORT SearchDialog : public AppDialog, private Ui::SearchDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(const QString &path, Git::Manager *git, QWidget *parent = nullptr);
    explicit SearchDialog(Git::Manager *git, QWidget *parent = nullptr);

    void initModel();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonSearchClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewDoubleClicked(const QModelIndex &index);
    LIBKOMMITWIDGETS_NO_EXPORT void beginSearch();
    LIBKOMMITWIDGETS_NO_EXPORT void searchOnPlace(const QString &place, const QString &commit);

    // QObject interface
protected:
    void timerEvent(QTimerEvent *event) override;

private:
    struct {
        int value;
        int total;
        QString message;
    } mProgress;
    QStandardItemModel *mModel{nullptr};
};
