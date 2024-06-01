/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <gitmanager.h>

class RepositoryData : public QObject
{
    Q_OBJECT

public:
    explicit RepositoryData(QObject *parent = nullptr);
    ~RepositoryData() override;
    void loadAll();

private:
    Git::Manager *const mManager{nullptr};
    Git::RemotesModel *const mRemotesModel{nullptr};
    Git::SubmodulesModel *const mSubmodulesModel{nullptr};
    Git::BranchesModel *const mBranchesModel{nullptr};
    Git::LogsModel *const mLogsCache{nullptr};
    Git::StashesModel *const mStashesCache{nullptr};
    Git::TagsModel *const mTagsModel{nullptr};
};
