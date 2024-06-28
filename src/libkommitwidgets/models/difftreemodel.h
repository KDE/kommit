/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "filestatus.h"
#include <diff.h>
#define TREEMODEL_NODE_DATA_TYPE Diff::DiffType
#include "treemodel.h"
#include <entities/treediff.h>

class DiffTreeModel : public TreeModel
{
    Q_OBJECT

public:
    explicit DiffTreeModel(QObject *parent = nullptr);

    void addFile(const Git::FileStatus &file);
    void addFile(const QString &file, Diff::DiffType type);
    void addFile(const Git::TreeDiffEntry &diffEntry);

    QVariant data(const QModelIndex &index, int role) const override;

    void emitAll();

private:
    [[nodiscard]] Diff::DiffType toDiffType(Git::FileStatus::Status status) const;
    [[nodiscard]] Diff::DiffType toDiffType(Git::ChangeStatus status) const;
    Node *createPath(const QStringList &path, Diff::DiffType status);
};
