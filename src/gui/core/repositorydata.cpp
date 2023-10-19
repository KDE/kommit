/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "repositorydata.h"

#include <models/authorsmodel.h>
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
    , mAuthorsModel{new Git::AuthorsModel{mManager, this}}
    , mSubmodulesModel{new Git::SubmodulesModel{mManager, this}}
    , mBranchesModel{new Git::BranchesModel{mManager, this}}
    , mLogsCache{new Git::LogsModel{mManager, mAuthorsModel, this}}
    , mStashesCache{new Git::StashesModel{mManager, this}}
    , mTagsModel{new Git::TagsModel{mManager, this}}
{
}

void RepositoryData::loadAll()
{
    mRemotesModel->load();
    mAuthorsModel->load();
    mSubmodulesModel->load();
    mBranchesModel->load();
    mLogsCache->load();
    mStashesCache->load();
    mTagsModel->load();
}
