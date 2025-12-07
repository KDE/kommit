/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "remotesactions.h"
#include "caches/remotescache.h"
#include "entities/remote.h"
#include "libkommitwidgets_appdebug.h"

#include <KLocalizedString>

#include <QInputDialog>

#include "core/kmessageboxhelper.h"
#include "dialogs/remoteinfodialog.h"
#include "dialogs/runnerdialog.h"
#include "models/remotesmodel.h"
#include "repository.h"

RemotesActions::RemotesActions(Git::Repository *git, QWidget *parent)
    : AbstractActions(git, parent)
{
    _actionCreate = addActionHidden(i18nc("@action", "New…"), this, &RemotesActions::create);
    _actionRemove = addActionDisabled(i18nc("@action", "Remove…"), this, &RemotesActions::remove);
    _actionRename = addActionDisabled(i18nc("@action", "Rename…"), this, &RemotesActions::rename);
    _actionChangeUrl = addActionDisabled(i18nc("@action", "Change url…"), this, &RemotesActions::changeUrl);
    _actionUpdate = nullptr;
    _actionCreate->setEnabled(true);
}

const QString &RemotesActions::remoteName() const
{
    return mRemoteName;
}

void RemotesActions::setRemoteName(const QString &newRemoteName)
{
    mRemoteName = newRemoteName;

    setActionEnabled(_actionRename, true);
    setActionEnabled(_actionChangeUrl, true);
}

void RemotesActions::create()
{
    RemoteInfoDialog d{mParent};
    if (d.exec() == QDialog::Accepted) {
        auto ok = mGit->remotes()->create(d.remoteName(), d.remoteUrl());

        if (!ok)
            KMessageBoxHelper::error(mParent, i18n("Unable to create remote"));
    }
}

void RemotesActions::remove()
{
    if (KMessageBoxHelper::removeQuestion(mParent, i18n("Are you sure to remove the selected remote?"), i18n("Remove remote?"))) {
        if (!mGit->remotes()->remove(mRemote)) {
            KMessageBoxHelper::information(mParent, i18n("Unable to remove the selected remote"));
            return;
        }
    }
}

void RemotesActions::changeUrl()
{
    auto remote = mGit->remotes()->findByName(mRemoteName);

    if (remote.isNull())
        return;

    const auto newUrl = QInputDialog::getText(mParent, i18nc("@title:window", "Change url"), i18n("URL"), QLineEdit::Normal, remote.pushUrl());
    if (!newUrl.isEmpty()) {
        mGit->remotes()->setUrl(mRemote, newUrl);
        KMessageBoxHelper::information(mParent, i18n("Url for remote changed successfully"));
    }
}

void RemotesActions::rename()
{
    const auto newName = QInputDialog::getText(mParent, i18nc("@title:window", "Rename"), i18n("New name"), QLineEdit::Normal, mRemoteName);

    if (!newName.isEmpty()) {
        mGit->remotes()->rename(mRemote, newName);
        mRemoteName = newName;
    }
}

void RemotesActions::update()
{
    // TODO implement it.
    qCWarning(KOMMIT_WIDGETS_LOG()) << "RemotesActions::update not implemented";
}

const Git::Remote &RemotesActions::remote() const
{
    return mRemote;
}

void RemotesActions::setRemote(const Git::Remote &newRemote)
{
    mRemote = newRemote;
}

#include "moc_remotesactions.cpp"
