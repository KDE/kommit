/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "stashactions.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QInputDialog>

#include "diffwindow.h"
#include "gitmanager.h"
#include "models/stashesmodel.h"
#include <kwidgetsaddons_version.h>

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
    setActionEnabled(_actionDrop, true);
}

void StashActions::apply()
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(mParent,
                                             i18n("Are you sure to apply the selected stash?"),
                                             i18n("Apply stash %1", mStashName),
                                             KStandardGuiItem::apply(),
                                             KStandardGuiItem::cancel());
#else
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to apply the selected stash?"), i18n("Apply stash %1", mStashName));
#endif

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    if (r == KMessageBox::ButtonCode::PrimaryAction)
#else
    if (r == KMessageBox::Yes)
#endif
        mGit->applyStash(mStashName);
}

void StashActions::drop()
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(mParent,
                                             i18n("Are you sure to apply the selected stash?"),
                                             i18n("Apply stash %1", mStashName),
                                             KStandardGuiItem::apply(),
                                             KStandardGuiItem::cancel());
#else
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to apply the selected stash?"), i18n("Apply stash %1", mStashName));
#endif

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    if (r == KMessageBox::ButtonCode::PrimaryAction) {
#else
    if (r == KMessageBox::Yes) {
#endif
        mGit->removeStash(mStashName);
        mGit->stashesModel()->load();
    }
}

void StashActions::pop()
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(mParent,
                                             i18n("Are you sure to apply the selected stash?"),
                                             i18n("Apply stash %1", mStashName),
                                             KStandardGuiItem::apply(),
                                             KStandardGuiItem::cancel());
#else
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to apply the selected stash?"), i18n("Apply stash %1", mStashName));
#endif

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    if (r == KMessageBox::ButtonCode::PrimaryAction) {
#else
    if (r == KMessageBox::Yes) {
#endif
        mGit->runGit({QStringLiteral("stash"), QStringLiteral("push"), mStashName});
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
    }
}
