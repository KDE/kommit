/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "repositorydata.h"

#include <models/branchesmodel.h>
#include <models/logsmodel.h>
#include <models/remotesmodel.h>
#include <models/stashesmodel.h>
#include <models/submodulesmodel.h>
#include <models/tagsmodel.h>

#include <gitmanager.h>

RepositoryData::RepositoryData(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Manager}
    , mRemotesModel{new Git::RemotesModel{mManager, this}}
    , mSubmodulesModel{new Git::SubmodulesModel{mManager, this}}
    , mBranchesModel{new Git::BranchesModel{mManager, this}}
    , mLogsCache{new Git::LogsModel{mManager, this}}
    , mStashesCache{new Git::StashesModel{mManager, this}}
    , mTagsModel{new Git::TagsModel{mManager, this}}
{
}

RepositoryData::~RepositoryData()
{
    delete mManager;
}

void RepositoryData::loadAll()
{
    mRemotesModel->load();
    mSubmodulesModel->load();
    mBranchesModel->load();
    mLogsCache->load();
    mStashesCache->load();
    mTagsModel->load();
}

#include "moc_repositorydata.cpp"
