/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "appdialog.h"
#include "libkommitwidgets_export.h"
#include "ui_searchdialog.h"

#include <Kommit/Tree>

#include <QReadWriteLock>

namespace Git
{
class Repository;
class Commit;
}

class QStandardItemModel;
class LIBKOMMITWIDGETS_EXPORT SearchDialog : public AppDialog, private Ui::SearchDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(const QString &path, Git::Repository *git, QWidget *parent = nullptr);
    explicit SearchDialog(Git::Repository *git, QWidget *parent = nullptr);

    void initModel();

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotPushButtonSearchClicked();
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewDoubleClicked(const QModelIndex &index);
    LIBKOMMITWIDGETS_NO_EXPORT void beginSearch();
    LIBKOMMITWIDGETS_NO_EXPORT void searchOnTree(const Git::Tree &tree, const QString &place);
    LIBKOMMITWIDGETS_NO_EXPORT void addSearchResult(const Git::Blob &file);
    Q_INVOKABLE LIBKOMMITWIDGETS_NO_EXPORT void searchFinished();

    struct SearchResult {
        Git::Blob file;
        QString place;
    };
    struct {
        int value;
        int total;
        int found;
        bool isCommit;
        QString message;
        QString currentPlace;
        QList<SearchResult> foundFiles;
    } mProgress;
    int mTimerId;
    QReadWriteLock mResultsLock;
    QStandardItemModel *const mModel;
};
