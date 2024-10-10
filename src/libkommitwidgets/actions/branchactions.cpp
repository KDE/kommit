/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchactions.h"

#include <KLocalizedString>

#include "caches/branchescache.h"
#include "libkommitwidgets_appdebug.h"
#include <QInputDialog>

#include <commands/commandmerge.h>
#include <entities/branch.h>
#include <repository.h>

#include "core/kmessageboxhelper.h"
#include "dialogs/fetchdialog.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/runnerdialog.h"
#include "models/branchesmodel.h"
#include "windows/diffwindow.h"

BranchActions::BranchActions(Git::Repository *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18nc("@action", "Create…"), this, &BranchActions::create);
    _actionCreate->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));

    _actionFetch = addActionDisabled(i18nc("@action", "Fetch…"), this, &BranchActions::fetch);
    _actionBrowse = addActionDisabled(i18nc("@action", "Browse…"), this, &BranchActions::browse);
    _actionCheckout = addActionDisabled(i18nc("@action", "Switch"), this, &BranchActions::checkout);
    _actionMerge = addActionDisabled(i18nc("@action", "Merge…"), this, &BranchActions::merge);
    _actionDiff = addActionDisabled(i18nc("@action", "Diff"), this, &BranchActions::diff);
    _actionRemove = addActionDisabled(i18nc("@action", "Remove…"), this, &BranchActions::remove);
}

void BranchActions::setBranch(QSharedPointer<Git::Branch> newBranch)
{
    mBranch = newBranch;

    setActionEnabled(_actionFetch, !newBranch.isNull());
    setActionEnabled(_actionBrowse, !newBranch.isNull());
    setActionEnabled(_actionCheckout, !newBranch.isNull());
    setActionEnabled(_actionMerge, !newBranch.isNull());
    setActionEnabled(_actionDiff, !newBranch.isNull());
    setActionEnabled(_actionRemove, !newBranch.isNull());
}

void BranchActions::setOtherBranch(QSharedPointer<Git::Branch> newOtherBranch)
{
    mOtherBranch = newOtherBranch;
}

void BranchActions::fetch()
{
    FetchDialog d(mGit, mParent);
    d.setBranch(mBranch->name());
    d.exec();
}

void BranchActions::create()
{
    const auto newBranchName = QInputDialog::getText(mParent, i18nc("@title:window", "Create new branch"), i18n("Branch name"));

    if (!newBranchName.isEmpty()) {
        mGit->branches()->create(newBranchName);
    }
}

void BranchActions::browse()
{
    FilesTreeDialog d(mGit, mBranch, mParent);
    d.exec();
}

void BranchActions::checkout()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("checkout"), mBranch->name()});
    d.exec();
}

void BranchActions::diff()
{
    // QString branchToDiff = mOtherBranch->name();

    // if (branchToDiff.isEmpty()) {
    //     auto currentBranch = mGit->branches()->currentName();

    //     if (currentBranch != branchToDiff) {
    //         branchToDiff = currentBranch;
    //     } else {
    //         auto branches = mGit->branches()->names(Git::BranchType::LocalBranch);
    //         if (branches.contains(QStringLiteral("master")))
    //             branchToDiff = QStringLiteral("master");
    //         else if (branches.contains(QStringLiteral("main")))
    //             branchToDiff = QStringLiteral("main");
    //     }
    //     if (branchToDiff.isEmpty()) {
    //         qCWarning(KOMMIT_WIDGETS_LOG()) << "Main branch is not set to diff";
    //         return;
    //     }
    // }

    auto d = new DiffWindow(mGit, mBranch, mOtherBranch);
    d->showModal();
}

void BranchActions::remove()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to remove the selected branch?"), i18n("Remove Branch"))) {
        if (!mGit->branches()->remove(mBranch))
            KMessageBoxHelper::information(mParent, i18n("Unable to remove the selected branch"));
    }
}

void BranchActions::merge()
{
    MergeDialog d{mGit, mBranch->name(), mParent};
    if (d.exec() == QDialog::Accepted) {
        auto cmd = d.command();
        RunnerDialog runner(mGit, mParent);
        runner.run(cmd);
        runner.exec();
    }
}

#include "moc_branchactions.cpp"
