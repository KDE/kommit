/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <gitmanager.h>

class RemotesModel;
class BranchesModel;
class CommitsModel;
class StashesModel;
class TagsModel;
class SubmodulesModel;

class RepositoryData : public QObject
{
    Q_OBJECT

public:
    explicit RepositoryData(Git::Manager *git);
    ~RepositoryData() override;
    void loadAll();

    Q_REQUIRED_RESULT Git::Manager *manager() const;

    Q_REQUIRED_RESULT SubmodulesModel *submodulesModel() const;

    Q_REQUIRED_RESULT RemotesModel *remotesModel() const;
    Q_REQUIRED_RESULT BranchesModel *branchesModel() const;
    Q_REQUIRED_RESULT CommitsModel *commitsModel() const;
    Q_REQUIRED_RESULT StashesModel *stashesModel() const;
    Q_REQUIRED_RESULT TagsModel *tagsModel() const;

private:
    Git::Manager *const mManager;
    RemotesModel *const mRemotesModel;
    SubmodulesModel *const mSubmodulesModel;
    BranchesModel *const mBranchesModel;
    CommitsModel *const mLogsCache;
    StashesModel *const mStashesCache;
    TagsModel *const mTagsModel;
};
