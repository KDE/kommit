/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleactions.h"
#include "core/kmessageboxhelper.h"
#include "dialogs/runnerdialog.h"
#include "dialogs/submoduleinfodialog.h"

#include "entities/submodule.h"

#include <caches/submodulescache.h>
#include <options/addsubmoduleoptions.h>
#include <repository.h>

#include <KLocalizedString>
#include <QAction>
#include <QDir>

SubmoduleActions::SubmoduleActions(Git::Repository *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18nc("@action", "Add…"), this, &SubmoduleActions::create);
    _actionInit = addAction(i18nc("@action", "Init…"), this, &SubmoduleActions::init, false);
    _actionUpdate = addAction(i18nc("@action", "Update…"), this, &SubmoduleActions::update, false);
    //    _actionDeinit = addAction(i18nc("@action", "Remove…"), this, &SubmoduleActions::deinit, false);
    _actionSync = addAction(i18nc("@action", "Sync…"), this, &SubmoduleActions::sync, false);

    _actionCreate->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));
}

void SubmoduleActions::init()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("init"), mSubmodule.name()});
    d.exec();
}

void SubmoduleActions::update()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("update"), mSubmodule.name()});
    d.exec();

    // Git::FetchOptions options;
    // auto observer = new Git::FetchObserver{mGit};

    // auto w = new FetchResultWidget;
    // w->setObserver(observer);
    // w->setWindowModality(Qt::ApplicationModal);

    // mSubmodule->update(options, observer);

    // w->show();
}

void SubmoduleActions::create()
{
    SubmoduleInfoDialog d(mGit, mParent);
    if (d.exec() == QDialog::Accepted) {
        Git::AddSubmoduleOptions opts;
        opts.setUrl(d.url());
        opts.setPath(d.path());
        mGit->submodules()->add(&opts);
    }
}

void SubmoduleActions::deinit()
{
    // if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to remove the selected submodule?"), i18n("Remove Submodule"))) {
    //     qCDebug(KOMMIT_WIDGETS_LOG()) << mGit->runGit(
    //         {QStringLiteral("submodule"), QStringLiteral("deinit"), QStringLiteral("-f"), QStringLiteral("--"), mSubModuleName});
    //     qCDebug(KOMMIT_WIDGETS_LOG()) << mGit->runGit({QStringLiteral("rm"), mSubModuleName});

    //     QDir d(mGit->path() + QStringLiteral("/.git/modules/") + mSubModuleName);
    //     if (!d.removeRecursively()) {
    //         KMessageBoxHelper::error(mParent, i18n("Unable to remove the module directory"));
    //         return;
    //     }
    //     qCDebug(KOMMIT_WIDGETS_LOG()) << d.path();
    //     mGit->runGit({QStringLiteral("config"), QStringLiteral("--remove-section"), QStringLiteral("submodule.") + mSubModuleName});

    //     mGit->submodulesModel()->load();
    //     KMessageBoxHelper::information(mParent, i18n("The submodule %1 removed", mSubModuleName));
    // }
}

void SubmoduleActions::sync()
{
    RunnerDialog d(mGit, mParent);
    d.run({QStringLiteral("submodule"), QStringLiteral("sync"), mSubmodule.name()});
    d.exec();
}

const Git::Submodule &SubmoduleActions::submodule() const
{
    return mSubmodule;
}

void SubmoduleActions::setSubmodule(const Git::Submodule &newSubmodule)
{
    mSubmodule = newSubmodule;

    _actionInit->setEnabled(!newSubmodule.isNull());
    _actionUpdate->setEnabled(!newSubmodule.isNull());
    _actionSync->setEnabled(!newSubmodule.isNull());
}

#include "moc_submoduleactions.cpp"
