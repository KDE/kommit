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

    const QIcon icon = Git::statusIcon(status);
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
    const auto buffer = QString(mGit->runGit({QStringLiteral("status"), QStringLiteral("--short")})).split(QLatin1Char('\n'));

    mData.clear();
    mData.reserve(buffer.size());

    for (const auto &line : buffer) {
        if (!line.trimmed().size())
            continue;

        auto status0 = line.mid(0, 1).trimmed();
        auto status1 = line.mid(1, 1).trimmed();
        const auto fileName = line.mid(3);
        Row d;
        d.filePath = fileName;

        if (status1 == QLatin1Char('M') || status0 == QLatin1Char('M'))
            d.status = Git::ChangeStatus::Modified;
        else if (status1 == QLatin1Char('A'))
            d.status = Git::ChangeStatus::Added;
        else if (status1 == QLatin1Char('D') || status0 == QLatin1Char('D'))
            d.status = Git::ChangeStatus::Removed;
        else if (status1 == QLatin1Char('C'))
            d.status = Git::ChangeStatus::Copied;
        else if (status1 == QLatin1Char('U'))
            d.status = Git::ChangeStatus::UpdatedButInmerged;
        else if (status1 == QLatin1Char('?'))
            d.status = Git::ChangeStatus::Untracked;
        else if (status1 == QLatin1Char('!'))
            d.status = Git::ChangeStatus::Ignored;
        else if (status1 == QLatin1Char('R')) {
            auto parts = fileName.split(QStringLiteral("->"));
            d.oldFilePath = parts[0].trimmed();
            d.filePath = parts[1].trimmed();
            d.status = Git::ChangeStatus::Renamed;
        } else {
            qDebug() << __FUNCTION__ << "The status" << status1 << "is unknown for" << fileName;
            d.status = Git::ChangeStatus::Unknown;
        }
        mData.append(d);
        createIcon(d.status);
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
        if (row.checked) {
            list << row.filePath;
            if (!row.oldFilePath.isEmpty())
                list << row.oldFilePath;
        }
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
