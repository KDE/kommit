/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "git/filestatus.h"
#include <QAbstractListModel>

#include <QList>
#include <QPair>
#include <QString>

class FilesModel : public QAbstractListModel
{
    Q_OBJECT

    QList<QPair<QString, QString>> _files;

public:
    explicit FilesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    void append(const QString &data);

    void addFile(const FileStatus &file);
};
