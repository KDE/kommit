/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "changedfilesmodel.h"

#include "caches/submodulescache.h"
#include "repository.h"

#include <libkommitwidgets_appdebug.h>

#include <KLocalizedString>

#include <QDebug>
#include <QIcon>

namespace Impl
{
QIcon statusIcon(Git::ChangeStatus status)
{
    switch (status) {
    case Git::ChangeStatus::Added:
        return QIcon::fromTheme(QStringLiteral("git-status-added"));
    case Git::ChangeStatus::Ignored:
        return QIcon::fromTheme(QStringLiteral("git-status-ignored"));
    case Git::ChangeStatus::Modified:
        return QIcon::fromTheme(QStringLiteral("git-status-modified"));
    case Git::ChangeStatus::Removed:
        return QIcon::fromTheme(QStringLiteral("git-status-removed"));
    case Git::ChangeStatus::Renamed:
        return QIcon::fromTheme(QStringLiteral("git-status-renamed"));
        //    case ChangeStatus::Unknown:
    case Git::ChangeStatus::Untracked:
        return QIcon::fromTheme(QStringLiteral("git-status-unknown"));
    case Git::ChangeStatus::Copied:
    case Git::ChangeStatus::UpdatedButInmerged:
    case Git::ChangeStatus::Unmodified:
        return QIcon::fromTheme(QStringLiteral("git-status-update"));
    case Git::ChangeStatus::Unknown:
        return {};
    default:
        qCWarning(KOMMIT_WIDGETS_LOG) << "Unknown icon" << (int)status;
    }
    return QIcon::fromTheme(QStringLiteral("git-status-update"));
}
}

void ChangedFilesModel::createIcon(Git::ChangeStatus status)
{
    if (mIcons.contains(status))
        return;

    const QIcon icon = Impl::statusIcon(status);
    mIcons.insert(status, icon);
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

ChangedFilesModel::ChangedFilesModel(Git::Repository *git, bool checkable, QObject *parent)
    : QAbstractListModel(parent)
    , mGit(git)
    , mCheckable(checkable)
{
}

void ChangedFilesModel::reload()
{
    beginResetModel();

    mData.clear();

    auto submodules = mGit->submodules()->allSubmodules();
    for (auto const &submodule : std::as_const(submodules)) {
        using Status = Git::Submodule::Status;
        auto status = submodule->status();

        if (status & Status::WdWdModified || status & Status::WdIndexModified || status & Status::WdModified) {
            Row d;
            d.filePath = submodule->name();
            d.status = Git::ChangeStatus::Modified;
            d.checked = true;
            d.submodule = submodule;

            createIcon(d.status);
            mData << d;
        }
    }

    auto files = mGit->changedFiles();
    for (auto i = files.begin(); i != files.end(); ++i) {
        if (i.value() == Git::ChangeStatus::Ignored)
            continue;

        Row d;
        d.filePath = i.key();
        d.status = i.value();
        d.checked = true;

        createIcon(d.status);
        mData << d;
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
    case Qt::DecorationRole: {
        auto icon = mIcons.value(row.status);
        if (icon.isNull())
            qDebug() << "No icon for" << (int)row.status << row.filePath;
        return icon;
    }

    case Qt::ForegroundRole:
        return row.submodule ? QVariant::fromValue(QColor{Qt::gray}) : QVariant{};

    case Qt::CheckStateRole: {
        if (mCheckable)
            return row.submodule.isNull() ? (row.checked ? Qt::Checked : Qt::Unchecked) : Qt::PartiallyChecked;
        else
            return {};
    }

    case Qt::DisplayRole: {
        if (index.column() == 1) {
            return row.filePath;
        }
        if (row.submodule) {
            if (row.submodule->status() & Git::Submodule::Status::WdModified)
                return row.filePath + i18n(" (new commit)");
            if (row.submodule->status() & Git::Submodule::Status::WdWdModified)
                return row.filePath + i18n(" (content modified)");
        }
        return row.filePath;
    }
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

const ChangedFilesModel::Row *ChangedFilesModel::data(int index) const
{
    return &mData.at(index);
}

int ChangedFilesModel::checkedCount() const
{
    int ret{0};
    for (const auto &row : mData)
        if (row.checked)
            ret++;
    return ret;
}

#include "moc_changedfilesmodel.cpp"
