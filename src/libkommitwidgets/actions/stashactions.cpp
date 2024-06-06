/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashactions.h"

#include <KLocalizedString>

#include <QInputDialog>

#include "core/kmessageboxhelper.h"
#include "gitmanager.h"
#include "models/stashesmodel.h"
#include "windows/diffwindow.h"
#include <entities/commit.h>

StashActions::StashActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionNew = addActionHidden(i18nc("@action", "New stash..."), this, &StashActions::create);
    _actionDiff = addAction(i18nc("@action", "Diff with working dir"), this, &StashActions::diff);
    _actionPop = addAction(i18nc("@action", "Pop..."), this, &StashActions::pop, false, true);
    _actionApply = addAction(i18nc("@action", "Apply..."), this, &StashActions::apply, false, true);
    _actionDrop = addAction(i18nc("@action", "Remove..."), this, &StashActions::drop, false, true);

    _actionDrop->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));
    _actionNew->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
}

void StashActions::apply()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to apply the selected stash?"), i18n("Apply stash %1", mStash->message()))) {
        if (!mGit->applyStash(mStash))
            KMessageBoxHelper::information(mParent, i18n("Unable to apply the selected stash"));
    }
}

void StashActions::drop()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to drop the selected stash?"), i18n("Drop stash %1", mStash->message()))) {
        if (!mGit->removeStash(mStash)) {
            KMessageBoxHelper::information(mParent, i18n("Unable to remove the selected stash"));
            return;
        }
        mGit->stashesModel()->load();
    }
}

void StashActions::pop()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to pop the selected stash?"), i18n("Pop stash %1", mStash->message()))) {
        if (!mGit->popStash(mStash)) {
            KMessageBoxHelper::information(mParent, i18n("Unable to remove the selected stash"));
            return;
        }
        mGit->stashesModel()->load();
    }
}

void StashActions::diff()
{
    auto d = new DiffWindow(mGit, mStash->commit()->tree());
    d->showModal();
}

void StashActions::create()
{
    if (mGit->changedFiles().empty()) {
        KMessageBoxHelper::information(mParent, i18n("You don't have any changes!"), i18n("Stash"));
        return;
    }
    bool ok;

    const auto name = QInputDialog::getText(mParent, i18n("Create new stash"), i18n("Name of stash"), QLineEdit::Normal, QString(), &ok);

    if (ok) {
        mGit->createStash(name);
        mGit->stashesModel()->load();
    }
}

QSharedPointer<Git::Stash> StashActions::stash() const
{
    return mStash;
}

void StashActions::setStash(QSharedPointer<Git::Stash> stash)
{
    mStash = stash;

    setActionEnabled(_actionApply, !stash.isNull());
    setActionEnabled(_actionDiff, !stash.isNull());
    setActionEnabled(_actionDrop, !stash.isNull());
    setActionEnabled(_actionPop, !stash.isNull());
}

#include "moc_stashactions.cpp"
