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
    Git::Manager *const mManager;
    Git::RemotesModel *const mRemotesModel;
    Git::SubmodulesModel *const mSubmodulesModel;
    Git::BranchesModel *const mBranchesModel;
    Git::LogsModel *const mLogsCache;
    Git::StashesModel *const mStashesCache;
    Git::TagsModel *const mTagsModel;
};
