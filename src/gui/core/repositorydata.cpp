/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "repositorydata.h"

#include "models/branchesmodel.h"
#include "models/commitsmodel.h"
#include "models/remotesmodel.h"
#include "models/stashesmodel.h"
#include "models/submodulesmodel.h"
#include "models/tagsmodel.h"

#include <Kommit/Repository>

RepositoryData::RepositoryData(Git::Repository *git)
    : QObject{git}
    , mManager{git} // Parented, as the three below already were: this owns every model it makes, and nothing
                    // else was deleting these ones at all.
    , mRemotesModel{new RemotesModel{mManager, this}}
    , mSubmodulesModel{new SubmodulesModel{mManager, this}}
    , mBranchesModel{new BranchesModel{mManager, this}}
    , mLogsCache{new CommitsModel{mManager, this}}
    , mStashesCache{new StashesModel{mManager, this}}
    , mTagsModel{new TagsModel{mManager, this}}
{
}

RepositoryData::~RepositoryData()
{
}

void RepositoryData::loadAll()
{
    mRemotesModel->load();
    mSubmodulesModel->reload();
    mBranchesModel->load();
    mLogsCache->load();
    mStashesCache->load();
    mTagsModel->load();
}

Git::Repository *RepositoryData::manager() const
{
    return mManager;
}

SubmodulesModel *RepositoryData::submodulesModel() const
{
    return mSubmodulesModel;
}

RemotesModel *RepositoryData::remotesModel() const
{
    return mRemotesModel;
}

BranchesModel *RepositoryData::branchesModel() const
{
    return mBranchesModel;
}

CommitsModel *RepositoryData::commitsModel() const
{
    return mLogsCache;
}

StashesModel *RepositoryData::stashesModel() const
{
    return mStashesCache;
}

TagsModel *RepositoryData::tagsModel() const
{
    return mTagsModel;
}

#include "moc_repositorydata.cpp"
