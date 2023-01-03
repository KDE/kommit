/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashactions.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QInputDialog>

#include "core/kmessageboxhelper.h"
#include "diffwindow.h"
#include "gitmanager.h"
#include "models/stashesmodel.h"

StashActions::StashActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionNew = addActionHidden(i18n("New stash..."), this, &StashActions::create);
    _actionDiff = addAction(i18n("Diff with HEAD..."), this, &StashActions::diff, false, true);
    _actionPop = addAction(i18n("Pop..."), this, &StashActions::pop, false, true);
    _actionApply = addAction(i18n("Apply..."), this, &StashActions::apply, false, true);
    _actionDrop = addAction(i18n("Remove..."), this, &StashActions::drop, false, true);

    _actionDrop->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));
    _actionNew->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
}

const QString &StashActions::stashName() const
{
    return mStashName;
}

void StashActions::setStashName(const QString &newStashName)
{
    mStashName = newStashName;

    setActionEnabled(_actionApply, true);
    setActionEnabled(_actionDiff, true);
    setActionEnabled(_actionDrop, true);
    setActionEnabled(_actionPop, true);
}

void StashActions::apply()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to apply the selected stash?"), i18n("Apply stash %1", mStashName))) {
        if (!mGit->applyStash(mStashName))
            KMessageBox::information(mParent, i18n("Unable to apply the selected stash"));
    }
}

void StashActions::drop()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to drop the selected stash?"), i18n("Drop stash %1", mStashName))) {
        if (!mGit->removeStash(mStashName)) {
            KMessageBox::information(mParent, i18n("Unable to remove the selected stash"));
            return;
        }
        mGit->stashesModel()->load();
    }
}

void StashActions::pop()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to pop the selected stash?"), i18n("Pop stash %1", mStashName))) {
        mGit->runGit({QStringLiteral("stash"), QStringLiteral("pop"), mStashName});
        mGit->stashesModel()->load();
    }
}

void StashActions::diff()
{
    auto d = new DiffWindow(mGit, mStashName, QStringLiteral("HEAD"));
    d->showModal();
}

void StashActions::create()
{
    if (mGit->changedFiles().empty()) {
        KMessageBox::information(mParent, i18n("You don't have any changes!"), i18n("Stash"));
        return;
    }
    bool ok;

    const auto name = QInputDialog::getText(mParent, i18n("Create new stash"), i18n("Name of stash"), QLineEdit::Normal, QString(), &ok);

    if (ok) {
        mGit->createStash(name);
        mGit->stashesModel()->load();
    }
}
