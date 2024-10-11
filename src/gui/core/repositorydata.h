/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

namespace Git
{
class Repository;
}

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
    explicit RepositoryData(Git::Repository *git);
    ~RepositoryData() override;
    void loadAll();

    [[nodiscard]] Git::Repository *manager() const;

    [[nodiscard]] SubmodulesModel *submodulesModel() const;

    [[nodiscard]] RemotesModel *remotesModel() const;
    [[nodiscard]] BranchesModel *branchesModel() const;
    [[nodiscard]] CommitsModel *commitsModel() const;
    [[nodiscard]] StashesModel *stashesModel() const;
    [[nodiscard]] TagsModel *tagsModel() const;

private:
    Git::Repository *const mManager;
    RemotesModel *const mRemotesModel;
    SubmodulesModel *const mSubmodulesModel;
    BranchesModel *const mBranchesModel;
    CommitsModel *const mLogsCache;
    StashesModel *const mStashesCache;
    TagsModel *const mTagsModel;
};
