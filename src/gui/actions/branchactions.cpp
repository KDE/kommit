/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchactions.h"

#include <KLocalizedString>
#include <KMessageBox>

#include "kommit_appdebug.h"
#include <QDebug>
#include <QInputDialog>

#include <commands/commandmerge.h>
#include <entities/branch.h>

#include "core/kmessageboxhelper.h"
#include "dialogs/fetchdialog.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/notedialog.h"
#include "dialogs/runnerdialog.h"
#include "diffwindow.h"
#include "gitmanager.h"
#include "models/branchesmodel.h"

BranchActions::BranchActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18n("Create..."), this, &BranchActions::create);
    _actionCreate->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));

    _actionFetch = addActionDisabled(i18n("Fetch..."), this, &BranchActions::fetch);
    _actionBrowse = addActionDisabled(i18n("Browse..."), this, &BranchActions::browse);
    _actionCheckout = addActionDisabled(i18n("Switch"), this, &BranchActions::checkout);
    _actionMerge = addActionDisabled(i18n("Merge..."), this, &BranchActions::merge);
    _actionDiff = addActionDisabled(i18n("Diff"), this, &BranchActions::diff);
    _actionRemove = addActionDisabled(i18n("Remove..."), this, &BranchActions::remove);
    _actionNote = addActionDisabled(i18n("Note..."), this, &BranchActions::note);
}

void BranchActions::setBranchName(const QSharedPointer<Git::Branch> &newBranchName)
{
    mBranchName = newBranchName;

    setActionEnabled(_actionFetch, true);
    setActionEnabled(_actionBrowse, true);
    setActionEnabled(_actionCheckout, true);
    setActionEnabled(_actionMerge, true);
    setActionEnabled(_actionDiff, true);
    setActionEnabled(_actionRemove, true);
    setActionEnabled(_actionNote, true);
}

void BranchActions::setOtherBranch(const QSharedPointer<Git::Branch> &newOtherBranch)
{
    mOtherBranch = newOtherBranch;
}

void BranchActions::fetch()
{
    FetchDialog d(mGit, mParent);
    d.setBranch(mBranchName->name());
    d.exec();
}

void BranchActions::create()
{
    const auto newBranchName = QInputDialog::getText(mParent, i18n("Create new branch"), i18n("Branch name"));

    if (!newBranchName.isEmpty()) {
        mGit->runGit({QStringLiteral("checkout"), QStringLiteral("-b"), newBranchName});
        mGit->branchesModel()->load();
    }
}

void BranchActions::browse()
{
    FilesTreeDialog d(mGit, mBranchName->name(), mParent);
    d.exec();
}

void BranchActions::checkout()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("checkout"), mBranchName->name()});
    d.exec();
}

void BranchActions::diff()
{
    QString branchToDiff = mOtherBranch->name();

    if (branchToDiff.isEmpty()) {
        auto currentBranch = mGit->currentBranch();

        if (currentBranch != branchToDiff) {
            branchToDiff = currentBranch;
        } else {
            auto branches = mGit->branchesNames(Git::Manager::BranchType::LocalBranch);
            if (branches.contains(QStringLiteral("master")))
                branchToDiff = QStringLiteral("master");
            else if (branches.contains(QStringLiteral("main")))
                branchToDiff = QStringLiteral("main");
        }
        if (branchToDiff.isEmpty()) {
            qCWarning(KOMMIT_LOG) << "Main branch is not set to diff";
            return;
        }
    }

    auto d = new DiffWindow(mGit, branchToDiff, mBranchName->name());
    d->showModal();
}

void BranchActions::remove()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to remove the selected branch?"), i18n("Remove Branch"))) {
        if (!mGit->removeBranch(mBranchName->name()))
            KMessageBox::information(mParent, i18n("Unable to remove the selected branch"));
        else
            mGit->branchesModel()->load();
    }
}

void BranchActions::merge()
{
    MergeDialog d{mGit, mBranchName->name(), mParent};
    if (d.exec() == QDialog::Accepted) {
        auto cmd = d.command();
        RunnerDialog runner(mGit, mParent);
        runner.run(cmd);
        runner.exec();
    }
}

void BranchActions::note()
{
    NoteDialog d{mGit, mBranchName->name(), mParent};
    d.exec();
}

#include "moc_branchactions.cpp"
