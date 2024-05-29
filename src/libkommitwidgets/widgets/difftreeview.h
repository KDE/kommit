/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_difftreeview.h"

#include "libkommitwidgets_export.h"

class QSortFilterProxyModel;
class DiffTreeModel;
class FilesModel;
class LIBKOMMITWIDGETS_EXPORT DiffTreeView : public QWidget, private Ui::DiffTreeView
{
    Q_OBJECT
    Q_PROPERTY(bool hideUnchangeds READ hideUnchangeds WRITE setHideUnchangeds NOTIFY hideUnchangedsChanged)

public:
    explicit DiffTreeView(QWidget *parent = nullptr);

    DiffTreeModel *diffModel() const;
    void setModels(DiffTreeModel *newDiffModel, FilesModel *filesModel);

    bool hideUnchangeds() const;
    void setHideUnchangeds(bool newHideUnchangeds);
    bool eventFilter(QObject *watched, QEvent *event) override;

Q_SIGNALS:
    void fileSelected(const QString &file);
    void hideUnchangedsChanged();

private:
    LIBKOMMITWIDGETS_NO_EXPORT void slotTreeViewClicked(const QModelIndex &index);
    LIBKOMMITWIDGETS_NO_EXPORT void slotListViewClicked(const QModelIndex &index);
    LIBKOMMITWIDGETS_NO_EXPORT void lineEditFilterTextChanged(const QString &text);
    DiffTreeModel *mDiffModel{nullptr};
    QSortFilterProxyModel *const mFilterModel;
    FilesModel *mFilesModel = nullptr;
};
