/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filesmodel.h"

FilesModel::FilesModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mFiles.size();
}

int FilesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}

QVariant FilesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mFiles.size())
        return {};

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        auto row = mFiles[index.row()];

        if (index.column() == 1)
            return row.second;
        return row.first;
    }

    return {};
}

void FilesModel::append(const QString &data)
{
    const auto i = data.lastIndexOf(QLatin1Char('/'));
    if (i != -1)
        mFiles.append({data.mid(i + 1), data});
    else
        mFiles.append({data, data});
}

void FilesModel::addFile(const Git::FileStatus &file)
{
    Q_UNUSED(file)
}

#include "moc_filesmodel.cpp"
