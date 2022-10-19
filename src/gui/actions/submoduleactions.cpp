/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleactions.h"
#include "commands/addsubmodulecommand.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/submoduleinfodialog.h"
#include "gitmanager.h"
#include "models/submodulesmodel.h"

#include "gitklient_appdebug.h"
#include <QAction>

#include <KLocalizedString>
#include <KMessageBox>
#include <QDir>
#include <kwidgetsaddons_version.h>

const QString &SubmoduleActions::subModuleName() const
{
    return mSubModuleName;
}

void SubmoduleActions::setSubModuleName(const QString &newSubModuleName)
{
    mSubModuleName = newSubModuleName.trimmed();

    setActionEnabled(_actionInit, true);
    setActionEnabled(_actionUpdate, true);
    //    setActionEnabled(_actionDeinit, true);
    setActionEnabled(_actionSync, true);
}

SubmoduleActions::SubmoduleActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18n("Add..."), this, &SubmoduleActions::create);
    _actionInit = addAction(i18n("Init..."), this, &SubmoduleActions::init, false);
    _actionUpdate = addAction(i18n("Update..."), this, &SubmoduleActions::update, false);
    //    _actionDeinit = addAction(i18n("Remove..."), this, &SubmoduleActions::deinit, false);
    _actionSync = addAction(i18n("Sync..."), this, &SubmoduleActions::sync, false);

    _actionCreate->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
}

void SubmoduleActions::init()
{
    RunnerDialog d(mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("init"), mSubModuleName});
    d.exec();
}

void SubmoduleActions::update()
{
    RunnerDialog d(mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("update"), mSubModuleName});
    d.exec();
}

void SubmoduleActions::create()
{
    SubmoduleInfoDialog d(mGit, mParent);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner;
        runner.run(d.command());
        runner.exec();
        mGit->submodulesModel()->load();
    }
}

void SubmoduleActions::deinit()
{
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    auto r = KMessageBox::questionTwoActions(mParent,
                                             i18n("Are you sure to remove the selected submodule?"),
                                             i18n("Remove Submodule"),
                                             KStandardGuiItem::remove(),
                                             KStandardGuiItem::cancel());
#else
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to remove the selected submodule?"), i18n("Remove Submodule"));
#endif

#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
    if (r == KMessageBox::ButtonCode::SecondaryAction)
#else
    if (r == KMessageBox::No)
#endif
        return;

    qCDebug(GITKLIENT_LOG) << mGit->runGit({QStringLiteral("submodule"), QStringLiteral("deinit"), QStringLiteral("-f"), QStringLiteral("--"), mSubModuleName});
    qCDebug(GITKLIENT_LOG) << mGit->runGit({QStringLiteral("rm"), mSubModuleName});

    QDir d(mGit->path() + QStringLiteral("/.git/modules/") + mSubModuleName);
    if (!d.removeRecursively()) {
        KMessageBox::error(mParent, i18n("Unable to remove the module directory"));
        return;
    }
    qCDebug(GITKLIENT_LOG) << d.path();
    mGit->runGit({QStringLiteral("config"), QStringLiteral("--remove-section"), QStringLiteral("submodule.") + mSubModuleName});

    mGit->submodulesModel()->load();
    KMessageBox::information(mParent, i18n("The submodule %1 removed", mSubModuleName));
}

void SubmoduleActions::sync()
{
    RunnerDialog d(mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("sync"), mSubModuleName});
    d.exec();
}
