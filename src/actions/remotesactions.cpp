/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotesactions.h"

#include <QInputDialog>
#include <QDebug>
#include <klocalizedstring.h>
#include <kmessagebox.h>

#include "dialogs/runnerdialog.h"
#include "git/commands/commandaddremote.h"
#include "git/gitmanager.h"
#include "git/models/remotesmodel.h"
#include "widgets/remoteinfodialog.h"

RemotesActions::RemotesActions(Git::Manager *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18n("New..."), this, &RemotesActions::create);
    _actionRemove = addActionDisabled(i18n("Remove..."), this, &RemotesActions::remove);
    _actionRename = addActionDisabled(i18n("Rename..."), this, &RemotesActions::rename);
    _actionChangeUrl = addActionDisabled(i18n("Change url..."), this, &RemotesActions::changeUrl);
    _actionUpdate = nullptr;
}

const QString &RemotesActions::remoteName() const
{
    return mRemoteName;
}

void RemotesActions::setRemoteName(const QString &newRemoteName)
{
    mRemoteName = newRemoteName;

    setActionEnabled(_actionRemove, true);
    setActionEnabled(_actionRename, true);
    setActionEnabled(_actionChangeUrl, true);
}

void RemotesActions::create()
{
    RemoteInfoDialog d{mParent};
    if (d.exec() == QDialog::Accepted) {
        //        _git->addRemote(d.remoteName(), d.remoteUrl());

        RunnerDialog runner;
        runner.run(d.command());
        runner.exec();
        mGit->remotesModel()->load();
    }
}

void RemotesActions::remove()
{
    auto r = KMessageBox::questionYesNo(mParent, i18n("Are you sure to remove the selected remote?"), i18n("Remove remote?"));

    if (r == KMessageBox::Yes) {
        mGit->removeRemote(mRemoteName);
        mGit->remotesModel()->load();
    }
}

void RemotesActions::changeUrl()
{
    auto remote = mGit->remotesModel()->findByName(mRemoteName);

    if (!remote)
        return;
    const auto newUrl = QInputDialog::getText(mParent, i18n("Change url"), i18n("URL"), QLineEdit::Normal, remote->pushUrl);
    if (!newUrl.isEmpty()) {
        mGit->remotesModel()->setUrl(mRemoteName, newUrl);
        KMessageBox::information(mParent, i18n("Url for remote changed successfully"));
    }
}

void RemotesActions::rename()
{
    const auto newName = QInputDialog::getText(mParent, i18n("Rename"), i18n("New name"), QLineEdit::Normal, mRemoteName);

    if (!newName.isEmpty()) {
        mGit->remotesModel()->rename(mRemoteName, newName);
        mRemoteName = newName;
    }
}

void RemotesActions::update()
{
}
