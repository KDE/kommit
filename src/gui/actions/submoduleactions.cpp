/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleactions.h"
#include "commands/addsubmodulecommand.h"
#include "core/kmessageboxhelper.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/submoduleinfodialog.h"
#include "gitmanager.h"
#include "models/submodulesmodel.h"

#include "kommit_appdebug.h"
#include <QAction>

#include <KLocalizedString>
#include <KMessageBox>
#include <QDir>

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
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("init"), mSubModuleName});
    d.exec();
}

void SubmoduleActions::update()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("update"), mSubModuleName});
    d.exec();
}

void SubmoduleActions::create()
{
    SubmoduleInfoDialog d(mGit, mParent);
    if (d.exec() == QDialog::Accepted) {
        RunnerDialog runner(mGit);
        runner.run(d.command());
        runner.exec();
        mGit->submodulesModel()->load();
    }
}

void SubmoduleActions::deinit()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to remove the selected submodule?"), i18n("Remove Submodule"))) {
        qCDebug(KOMMIT_LOG) << mGit->runGit(
            {QStringLiteral("submodule"), QStringLiteral("deinit"), QStringLiteral("-f"), QStringLiteral("--"), mSubModuleName});
        qCDebug(KOMMIT_LOG) << mGit->runGit({QStringLiteral("rm"), mSubModuleName});

        QDir d(mGit->path() + QStringLiteral("/.git/modules/") + mSubModuleName);
        if (!d.removeRecursively()) {
            KMessageBox::error(mParent, i18n("Unable to remove the module directory"));
            return;
        }
        qCDebug(KOMMIT_LOG) << d.path();
        mGit->runGit({QStringLiteral("config"), QStringLiteral("--remove-section"), QStringLiteral("submodule.") + mSubModuleName});

        mGit->submodulesModel()->load();
        KMessageBox::information(mParent, i18n("The submodule %1 removed", mSubModuleName));
    }
}

void SubmoduleActions::sync()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("sync"), mSubModuleName});
    d.exec();
}
