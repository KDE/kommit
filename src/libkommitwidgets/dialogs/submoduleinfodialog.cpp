/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "submoduleinfodialog.h"

#include "commands/addsubmodulecommand.h"
#include "core/kmessageboxhelper.h"
#include "gitmanager.h"

#include <KLocalizedString>

#include <QFileDialog>

SubmoduleInfoDialog::SubmoduleInfoDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);
    lineEditPath->setStartDir(QUrl::fromLocalFile(mGit->path()));
    // lineEditPath->setMode(KFile::Directory);
    connect(lineEditPath, &UrlRequester::urlSelected, this, &SubmoduleInfoDialog::slotLineEditPathUrlSelected);
}

bool SubmoduleInfoDialog::force() const
{
    return checkBoxForce->isChecked();
}

void SubmoduleInfoDialog::setForce(bool newForce)
{
    checkBoxForce->setChecked(newForce);
}

QString SubmoduleInfoDialog::url() const
{
    return lineEditUrl->text();
}

void SubmoduleInfoDialog::setUrl(const QString &newUrl)
{
    lineEditUrl->setText(newUrl);
}

QString SubmoduleInfoDialog::path() const
{
    return lineEditPath->text();
}

void SubmoduleInfoDialog::setPath(const QString &newPath)
{
    lineEditPath->setText(newPath);
}

QString SubmoduleInfoDialog::branch() const
{
    if (checkBoxBranch->isChecked())
        return lineEditBranch->text();
    return {};
}

void SubmoduleInfoDialog::setBranch(const QString &newBranch)
{
    if (newBranch.isEmpty()) {
        checkBoxBranch->setChecked(true);
        lineEditBranch->setText(newBranch);
    } else {
        checkBoxBranch->setChecked(false);
        lineEditBranch->clear();
    }
}

Git::AddSubmoduleCommand *SubmoduleInfoDialog::command() const
{
    auto cmd = new Git::AddSubmoduleCommand(mGit);
    cmd->setForce(checkBoxForce->isChecked());

    if (checkBoxBranch->isChecked())
        cmd->setbranch(lineEditBranch->text());

    cmd->setUrl(lineEditUrl->text());
    cmd->setLocalPath(lineEditPath->text());

    return cmd;
}

void SubmoduleInfoDialog::slotLineEditPathUrlSelected(const QUrl &url)
{
    auto text = url.toLocalFile();
    if (text.isEmpty())
        return;

    if (text.startsWith(mGit->path())) {
        auto t = text;
        t.remove(0, mGit->path().size());
        if (t.startsWith("/"))
            t.remove(0, 1);
        lineEditPath->setText(t);
    } else {
        KMessageBoxHelper::error(this, i18n("The path is not inside of git directory"));
        lineEditPath->setText(QString{});
    }
}

#include "moc_submoduleinfodialog.cpp"
