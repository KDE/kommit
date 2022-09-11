/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "diff/diff.h"
#include "git/filestatus.h"
#define TREEMODEL_NODE_DATA_TYPE Diff::DiffType
#include "treemodel.h"

class DiffTreeModel : public TreeModel
{
    Q_OBJECT

public:
    explicit DiffTreeModel(QObject *parent = nullptr);

    void addFile(const FileStatus &file);
    void addFile(const QString &file, const Diff::DiffType &type);

    QVariant data(const QModelIndex &index, int role) const override;

    void emitAll();

private:
    Node *createPath(const QStringList &path, const Diff::DiffType &status);
    QColor statusColor(const Diff::DiffType &status) const;
    Diff::DiffType toDiffType(const FileStatus::Status &status);
};
