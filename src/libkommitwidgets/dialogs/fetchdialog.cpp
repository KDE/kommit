/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchdialog.h"

#include <Kommit/BranchesCache>
#include <Kommit/CommandFetch>
#include <Kommit/Credential>
#include <Kommit/Error>
#include <Kommit/Fetch>
#include <Kommit/FetchObserver>
#include <Kommit/Oid>
#include <Kommit/Reference>
#include <Kommit/RemoteCallbacks>
#include <Kommit/RemotesCache>
#include <Kommit/Repository>

#include <fetch.h>

#include "certificateinfodialog.h"
#include "credentialdialog.h"
#include "runnerdialog.h"

#include <QDialogButtonBox>
#include <QPushButton>

FetchDialog::FetchDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mObserver{new Git::FetchObserver{git}}
{
    setupUi(this);

    comboBoxRemote->addItems(git->remotes()->allNames());
    comboBoxBranch->addItems(git->branches()->names(Git::BranchType::LocalBranch));

    comboBoxRemote->setCurrentText(git->branches()->currentName());
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FetchDialog::slotAccept);

    stackedWidget->setCurrentIndex(0);
}

void FetchDialog::setBranch(const QString &branch)
{
    comboBoxBranch->setCurrentText(branch);
}

void FetchDialog::slotAccept()
{
    // Git::CommandFetch *cmd = new Git::CommandFetch;

    // cmd->setRemote(comboBoxRemote->currentText());

    // if (!checkBoxAllBranches->isChecked())
    //     cmd->setBranch(comboBoxBranch->currentText());
    // // cmd->setNoFf(checkBoxNoFastForward->isChecked());
    // // cmd->setFfOnly(checkBoxFastForwardOnly->isChecked());
    // // cmd->setNoCommit(checkBoxNoCommit->isChecked());
    // cmd->setPrune(checkBoxPrune->isChecked());
    // cmd->setTags(checkBoxTags->isChecked());

    // RunnerDialog d(mGit, this);
    // d.run(cmd);
    // d.exec();

    // accept();
    startFetch();
}

void FetchDialog::startFetch()
{
    mFetch = new Git::Fetch{mGit};

    auto remote = mGit->remotes()->findByName(comboBoxRemote->currentText());
    mFetch->setRemote(remote);

    // set prune
    switch (checkBoxPrune->checkState()) {
    case Qt::Unchecked:
        mFetch->setPrune(Git::Fetch::Prune::NoPrune);
        break;
    case Qt::PartiallyChecked:
        mFetch->setPrune(Git::Fetch::Prune::PruneUnspecified);
        break;
    case Qt::Checked:
        mFetch->setPrune(Git::Fetch::Prune::Prune);
        break;
    }

    // set download tags
    switch (checkBoxTags->checkState()) {
    case Qt::Unchecked:
        mFetch->setDownloadTags(Git::Fetch::DownloadTags::None);
        break;
    case Qt::PartiallyChecked:
        mFetch->setDownloadTags(Git::Fetch::DownloadTags::Auto);
        break;
    case Qt::Checked:
        mFetch->setDownloadTags(Git::Fetch::DownloadTags::All);
        break;
    }

    // set redirect
    switch (checkBoxRedirect->checkState()) {
    case Qt::Unchecked:
        mFetch->setRedirect(Git::Fetch::Redirect::None);
        break;
    case Qt::PartiallyChecked:
        mFetch->setRedirect(Git::Fetch::Redirect::Initial);
        break;
    case Qt::Checked:
        mFetch->setRedirect(Git::Fetch::Redirect::All);
        break;
    }

    // set depth
    if (checkBoxDepth->isChecked())
        mFetch->setDepth(spinBoxDepth->value());

    auto callbacks = mFetch->remoteCallbacks();

    connectRemoteCallbacks(callbacks);

    connect(callbacks, &Git::RemoteCallbacks::message, this, &FetchDialog::slotFetchMessage);
    connect(callbacks, &Git::RemoteCallbacks::transferProgress, this, &FetchDialog::slotFetchTransferProgress);
    connect(callbacks, &Git::RemoteCallbacks::packProgress, this, &FetchDialog::slotFetchPackProgress);
    connect(callbacks, &Git::RemoteCallbacks::updateRef, this, &FetchDialog::slotFetchUpdateRef);
    connect(mFetch, &Git::Fetch::finished, this, &FetchDialog::slotFetchFinished);

    stackedWidget->setCurrentIndex(1);
    buttonBox_2->button(QDialogButtonBox::Close)->setEnabled(false);

    qApp->processEvents();
    mIsChanged = false;
    mFetch->run();
    qApp->processEvents();

    buttonBox_2->button(QDialogButtonBox::Close)->setEnabled(true);
}

void FetchDialog::slotFetchMessage(const QString &message)
{
    labelStatus->setText(message);
}

void FetchDialog::slotFetchTransferProgress(const Git::FetchTransferStat *stat)
{
    labelStatus->setText(i18n("Receiving objects"));
    progressBar->setMaximum(stat->totalObjects);
    progressBar->setValue(stat->receivedObjects);
}

void FetchDialog::slotFetchPackProgress(const Git::PackProgress *p)
{
    labelStatus->setText(i18n("Stage %1", p->stage));
    progressBar->setMaximum(p->total);
    progressBar->setValue(p->current);
}

void FetchDialog::slotFetchUpdateRef(const Git::Reference &reference, const Git::Oid &a, const Git::Oid &b)
{
    mIsChanged = true;
    textBrowser->append(i18n("Reference updated: %1  %2...%3", reference.name(), a.toString(), b.toString()));
}

void FetchDialog::slotFetchFinished(bool success)
{
    if (success) {
        labelStatus->setText(i18n("Finished"));
    } else {
        labelStatus->setText(i18n("Finished with error"));
        textBrowser->append(i18n("Error %1: %2", Git::Error::klass(), Git::Error::message()));
    }

    progressBar->setValue(progressBar->maximum());
}

#include "moc_fetchdialog.cpp"
