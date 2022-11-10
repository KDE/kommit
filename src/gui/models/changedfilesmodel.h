/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "gitglobal.h"
#include <QAbstractListModel>

class ChangedFilesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    ChangedFilesModel(Git::Manager *git, bool checkable = false, QObject *parent = nullptr);
    void reload();

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QString filePath(int index) const;
    int size() const;
    QStringList checkedFiles() const;
    void checkByStatus(Git::ChangeStatus status);
    void checkByStatus(const QList<Git::ChangeStatus> &statuses);
    void toggleCheckAll(bool checked);

    struct Row {
        QString filePath;
        Git::ChangeStatus status;
        bool checked;
    };
    const QList<Row> &data() const;
    int checkedCount() const;

private:
    Git::Manager *mGit{nullptr};
    QList<Row> mData;
    QIcon createIcon(Git::ChangeStatus status);
    QMap<Git::ChangeStatus, QIcon> mIcons;

Q_SIGNALS:
    void checkedCountChanged();


private:
    bool mCheckable;
};
