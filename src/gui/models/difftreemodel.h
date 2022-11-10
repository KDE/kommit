/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "diff/diff.h"
#include "filestatus.h"
#define TREEMODEL_NODE_DATA_TYPE Diff::DiffType
#include "treemodel.h"

class DiffTreeModel : public TreeModel
{
    Q_OBJECT

public:
    explicit DiffTreeModel(QObject *parent = nullptr);

    void addFile(const FileStatus &file);
    void addFile(const QString &file, Diff::DiffType type);

    QVariant data(const QModelIndex &index, int role) const override;

    void emitAll();

private:
    Q_REQUIRED_RESULT Diff::DiffType toDiffType(FileStatus::Status status) const;
    Node *createPath(const QStringList &path, Diff::DiffType status);
    QColor statusColor(Diff::DiffType status) const;
};
