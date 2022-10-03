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

#include <QAction>
#include "gitklient_appdebug.h"

#include <QDir>
#include <klocalizedstring.h>
#include <kmessagebox.h>

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
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to remove the selected submodule?"));

    if (r == KMessageBox::No)
        return;

    qCDebug(GITKLIENT_LOG) << mGit->runGit({"submodule", "deinit", "-f", "--", mSubModuleName});
    qCDebug(GITKLIENT_LOG) << mGit->runGit({QStringLiteral("rm"), mSubModuleName});

    QDir d(mGit->path() + "/.git/modules/" + mSubModuleName);
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
