/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashactions.h"

#include <KLocalizedString>

#include <QInputDialog>

#include "caches/stashescache.h"
#include "core/kmessageboxhelper.h"
#include "entities/stash.h"
#include "models/stashesmodel.h"
#include "repository.h"
#include "windows/diffwindow.h"
#include <entities/commit.h>

StashActions::StashActions(Git::Repository *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionNew = addActionHidden(i18nc("@action", "New stash…"), this, &StashActions::create);
    _actionDiff = addAction(i18nc("@action", "Diff with working dir"), this, &StashActions::diff);
    _actionPop = addAction(i18nc("@action", "Pop…"), this, &StashActions::pop, false, true);
    _actionApply = addAction(i18nc("@action", "Apply…"), this, &StashActions::apply, false, true);
    _actionDrop = addAction(i18nc("@action", "Remove…"), this, &StashActions::drop, false, true);

    _actionDrop->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));
    _actionNew->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
}

void StashActions::apply()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to apply the selected stash?"), i18n("Apply stash %1", mStash->message()))) {
        if (!mGit->stashes()->apply(mStash))
            KMessageBoxHelper::information(mParent, i18n("Unable to apply the selected stash"));
    }
}

void StashActions::drop()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to drop the selected stash?"), i18n("Drop stash %1", mStash->message()))) {
        if (!mGit->stashes()->remove(mStash)) {
            KMessageBoxHelper::information(mParent, i18n("Unable to remove the selected stash"));
            return;
        }
    }
}

void StashActions::pop()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to pop the selected stash?"), i18n("Pop stash %1", mStash->message()))) {
        if (!mGit->stashes()->pop(mStash)) {
            KMessageBoxHelper::information(mParent, i18n("Unable to remove the selected stash"));
            return;
        }
    }
}

void StashActions::diff()
{
    auto d = new DiffWindow(mGit, mStash->commit());
    d->showModal();
}

void StashActions::create()
{
    if (mGit->changedFiles().empty()) {
        KMessageBoxHelper::information(mParent, i18n("You don't have any changes!"), i18nc("@title:window", "Stash"));
        return;
    }
    bool ok;

    const auto name = QInputDialog::getText(mParent, i18nc("@title:window", "Create new stash"), i18n("Name of stash"), QLineEdit::Normal, QString(), &ok);

    if (ok) {
        mGit->stashes()->create(name);
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
