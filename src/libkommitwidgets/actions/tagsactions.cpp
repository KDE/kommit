/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagsactions.h"

#include "core/kmessageboxhelper.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/taginfodialog.h"
#include "entities/tag.h"
#include "gitmanager.h"
#include "models/tagsmodel.h"

#include <QAction>

#include "windows/diffwindow.h"
#include <KLocalizedString>

TagsActions::TagsActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18nc("@action", "New tag…"), this, &TagsActions::create);
    _actionRemove = addActionDisabled(i18nc("@action", "Remove…"), this, &TagsActions::remove);
    _actionCheckout = addActionDisabled(i18nc("@action", "Checkout…"), this, &TagsActions::checkout);
    _actionDiff = addActionDisabled(i18nc("@action", "Diff with working dir"), this, &TagsActions::diff);
    _actionPush = addAction(i18nc("@action", "Push…"), this, &TagsActions::push);
}

void TagsActions::create()
{
    TagInfoDialog d(mParent);
    d.setWindowTitle(i18nc("@title:window", "New tag"));
    if (d.exec() == QDialog::Accepted) {
        mGit->createTag(d.tagName(), d.message());
        mGit->tagsModel()->load();
    }
}

void TagsActions::remove()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to remove the selected tag?"), i18nc("@title:window", "Remove tag"))) {
        mGit->runGit({QStringLiteral("tag"), QStringLiteral("-d"), mTag->name()});
        mGit->tagsModel()->load();
    }
}

void TagsActions::checkout()
{
    if (KMessageBoxHelper::applyQuestion(mParent, i18n("Are you sure to checkout to the selected tag?"), i18nc("@title:window", "Checkout"))) {
        mGit->runGit({QStringLiteral("tag"), QStringLiteral("checkout"), QStringLiteral("tags/") + mTag->name()});
    }
}

void TagsActions::diff()
{
    auto d = new DiffWindow(mGit, mTag);
    d->showModal();
}

void TagsActions::push()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("push"), QStringLiteral("--tags")});
    d.exec();
}

QSharedPointer<Git::Tag> TagsActions::tag() const
{
    return mTag;
}

void TagsActions::setTag(QSharedPointer<Git::Tag> tag)
{
    mTag = tag;

    setActionEnabled(_actionRemove, !tag.isNull());
    setActionEnabled(_actionCheckout, !tag.isNull());
    setActionEnabled(_actionDiff, !tag.isNull());
}

#include "moc_tagsactions.cpp"
