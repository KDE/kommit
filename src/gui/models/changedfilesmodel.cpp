/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfilesmodel.h"

#include "gitmanager.h"

#include <GitKlientSettings.h>
#include <QIcon>
#include <QPainter>
#include <QPixmap>

QIcon ChangedFilesModel::createIcon(Git::ChangeStatus status)
{
    if (mIcons.contains(status))
        return mIcons[status];

    QColor color;
    switch (status) {
    case Git::ChangeStatus::Modified:
        color = GitKlientSettings::diffModifiedColor();
        break;
    case Git::ChangeStatus::Added:
    case Git::ChangeStatus::Untracked:
        color = GitKlientSettings::diffAddedColor();
        break;
    case Git::ChangeStatus::Removed:
        color = GitKlientSettings::diffRemovedColor();
        break;

    default:
        return {};
    }
    QPixmap pixmap(32, 32);
    QPainter p(&pixmap);
    p.setBrush(color);
    p.setPen(color);
    p.fillRect(0, 0, 32, 32, Qt::color1);

    p.setPen(Qt::black);
    p.drawEllipse({16, 16}, 8, 8);

    QImage image = pixmap.toImage();
    image.setAlphaChannel(pixmap.toImage());

    QIcon icon(QPixmap::fromImage(image));
    mIcons.insert(status, icon);
    return icon;
}

bool ChangedFilesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (mCheckable && role == Qt::CheckStateRole && index.isValid()) {
        bool shouldEmit = mData[index.row()].checked != value.toBool();
        mData[index.row()].checked = value.toBool();
        if (shouldEmit)
            Q_EMIT checkedCountChanged();
        Q_EMIT dataChanged(index, index);
    }
    return QAbstractListModel::setData(index, value, role);
}

Qt::ItemFlags ChangedFilesModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);
    if (index.isValid()) {
        return defaultFlags | Qt::ItemIsUserCheckable;
    }
    return defaultFlags;
}

ChangedFilesModel::ChangedFilesModel(Git::Manager *git, bool checkable, QObject *parent)
    : QAbstractListModel(parent)
    , mGit(git)
    , mCheckable(checkable)
{
}

void ChangedFilesModel::reload()
{
    beginResetModel();
    auto changedFiles = mGit->changedFiles();
    mData.clear();
    mData.reserve(changedFiles.size());
    for (auto i = changedFiles.begin(); i != changedFiles.end(); ++i) {
        mData.append({i.key(), i.value(), false});
        createIcon(i.value());
    }
    endResetModel();
}

int ChangedFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return mData.size();
}

int ChangedFilesModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant ChangedFilesModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mData.size())
        return {};
    auto row = mData[index.row()];

    switch (role) {
    case Qt::DecorationRole:
        return mIcons.value(row.status);

    case Qt::CheckStateRole: {
        if (mCheckable)
            return row.checked ? Qt::Checked : Qt::Unchecked;
        else
            return {};
    }
    }

    if (role == Qt::DisplayRole) {
        if (index.column() == 1)
            return row.filePath;
        return row.filePath;
    }
    return {};
}

QString ChangedFilesModel::filePath(int index) const
{
    if (index < 0 || index >= mData.size())
        return {};
    return mData[index].filePath;
}

int ChangedFilesModel::size() const
{
    return mData.size();
}

QStringList ChangedFilesModel::checkedFiles() const
{
    QStringList list;
    for (auto &row : mData)
        if (row.checked)
            list << row.filePath;
    return list;
}

void ChangedFilesModel::checkByStatus(Git::ChangeStatus status)
{
    for (auto &row : mData)
        row.checked = row.status == status;
    Q_EMIT dataChanged(index(0), index(mData.size() - 1));
}

void ChangedFilesModel::checkByStatus(const QList<Git::ChangeStatus> &statuses)
{
    for (auto &row : mData)
        row.checked = statuses.contains(row.status);
    Q_EMIT dataChanged(index(0), index(mData.size() - 1));
}

void ChangedFilesModel::toggleCheckAll(bool checked)
{
    for (auto &row : mData)
        row.checked = checked;
    Q_EMIT dataChanged(index(0), index(mData.size() - 1));
}

const QList<ChangedFilesModel::Row> &ChangedFilesModel::data() const
{
    return mData;
}

int ChangedFilesModel::checkedCount() const
{
    int ret{0};
    for (const auto &row : mData)
        if (row.checked)
            ret++;
    return ret;
}
