/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "branchactions.h"

#include <KLocalizedString>
#include <kmessagebox.h>

#include <QInputDialog>

#include "dialogs/fetchdialog.h"
#include "dialogs/filestreedialog.h"
#include "dialogs/mergedialog.h"
#include "dialogs/notedialog.h"
#include "dialogs/runnerdialog.h"
#include "diffwindow.h"
#include "git/commands/commandmerge.h"
#include "git/gitmanager.h"
#include "git/models/branchesmodel.h"

BranchActions::BranchActions(Git::Manager *git, QWidget *parent) : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18n("Create..."), this, &BranchActions::create);
    _actionCreate->setIcon(QIcon::fromTheme("document-new"));

    _actionFetch = addActionDisabled(i18n("Fetch..."), this, &BranchActions::fetch);
    _actionBrowse = addActionDisabled(i18n("Browse..."), this, &BranchActions::browse);
    _actionCheckout = addActionDisabled(i18n("Switch"), this, &BranchActions::checkout);
    _actionMerge = addActionDisabled(i18n("Merge..."), this, &BranchActions::merge);
    _actionDiff = addActionDisabled(i18n("Diff"), this, &BranchActions::diff);
    _actionRemove = addActionDisabled(i18n("Remove..."), this, &BranchActions::remove);
    _actionNote = addActionDisabled(i18n("Note..."), this, &BranchActions::note);
}

const QString &BranchActions::branchName() const
{
    return _branchName;
}

void BranchActions::setBranchName(const QString &newBranchName)
{
    _branchName = newBranchName;

    setActionEnabled(_actionFetch, true);
    setActionEnabled(_actionBrowse, true);
    setActionEnabled(_actionCheckout, true);
    setActionEnabled(_actionMerge, true);
    setActionEnabled(_actionDiff, true);
    setActionEnabled(_actionRemove, true);
    setActionEnabled(_actionNote, true);
}

const QString &BranchActions::otherBranch() const
{
    return _otherBranch;
}

void BranchActions::setOtherBranch(const QString &newOtherBranch)
{
    _otherBranch = newOtherBranch;

}

void BranchActions::fetch()
{
    FetchDialog d(mGit, mParent);
    d.setBranch(_branchName);
    d.exec();
}

void BranchActions::create()
{
    const auto newBranchName = QInputDialog::getText(mParent, i18n("Create new branch"), i18n("Branch name"));

    if (!newBranchName.isEmpty()) {
        mGit->runGit({"checkout", "-b", newBranchName});
        mGit->branchesModel()->load();
    }
}

void BranchActions::browse()
{
    FilesTreeDialog d(_branchName, mParent);
    d.exec();
}

void BranchActions::checkout()
{
    RunnerDialog d(mParent);
    d.run({"checkout", _branchName});
    d.exec();
}

void BranchActions::diff()
{
    QString mainBranch = _otherBranch;

    if (!mainBranch.isEmpty()) {
        auto branches = mGit->branches();
        if (branches.contains(QStringLiteral("master")))
            mainBranch = QStringLiteral("master");
        else if (branches.contains(QStringLiteral("main")))
            mainBranch = QStringLiteral("main");
        else
            return;
    }

    auto d = new DiffWindow(mGit, mainBranch, _branchName);
    d->showModal();
}

void BranchActions::remove()
{
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to remove the selected branch?"), i18n("Remove Branch"));

    if (r == KMessageBox::No)
        return;

    mGit->removeBranch(_branchName);
}

void BranchActions::merge()
{
    MergeDialog d{mGit, _branchName, mParent};
    if (d.exec() == QDialog::Accepted) {
        auto cmd = d.command();
        RunnerDialog runner(mParent);
        runner.run(cmd);
        runner.exec();
    }
}

void BranchActions::note()
{
    NoteDialog d{mGit, _branchName, mParent};
    d.exec();
}
