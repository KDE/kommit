/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchdialog.h"

#include <Kommit/BranchesCache>
#include <Kommit/CommandFetch>
#include <Kommit/Credential>
#include <Kommit/Error>
#include <Kommit/FetchOptions>
#include <Kommit/Oid>
#include <Kommit/Reference>
#include <Kommit/RemoteCallbacks>
#include <Kommit/RemotesCache>
#include <Kommit/Repository>

#include "certificateinfodialog.h"
#include "credentialdialog.h"
#include "runnerdialog.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QtConcurrent/QtConcurrent>

FetchDialog::FetchDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mFetchOptions{new Git::FetchOptions{git}}
{
    setupUi(this);

    comboBoxRemote->addItems(git->remotes()->allNames());
    comboBoxBranch->addItems(git->branches()->names(Git::BranchType::LocalBranch));

    comboBoxRemote->setCurrentText(git->branches()->currentName());
    connect(buttonBox, &QDialogButtonBox::accepted, this, &FetchDialog::slotAccept);

    stackedWidget->setCurrentIndex(0);

    auto callbacks = mFetchOptions->remoteCallbacks();
    connect(callbacks, &Git::RemoteCallbacks::message, this, &FetchDialog::slotFetchMessage);
    connect(callbacks, &Git::RemoteCallbacks::transferProgress, this, &FetchDialog::slotFetchTransferProgress);
    connect(callbacks, &Git::RemoteCallbacks::packProgress, this, &FetchDialog::slotFetchPackProgress);
    connect(callbacks, &Git::RemoteCallbacks::updateRef, this, &FetchDialog::slotFetchUpdateRef);
    connect(callbacks, &Git::RemoteCallbacks::credentialRequested, this, &FetchDialog::slotCredentialRequested, Qt::BlockingQueuedConnection);
    connect(callbacks, &Git::RemoteCallbacks::certificateCheck, this, &FetchDialog::slotCertificateCheck, Qt::BlockingQueuedConnection);
    // connect(mFetchOptions, &Git::FetchOptions::finished, this, &FetchDialog::slotFetchFinished);
}

void FetchDialog::setBranch(const QString &branch)
{
    comboBoxBranch->setCurrentText(branch);
}

void FetchDialog::slotAccept()
{
    buttonBox_2->button(QDialogButtonBox::Close)->setEnabled(false);
    stackedWidget->setCurrentIndex(1);
    startFetch();
}

void FetchDialog::startFetch()
{
    auto remote = mGit->remotes()->findByName(comboBoxRemote->currentText());
    // mFetch->setRemote(remote);

    if (remote.isNull())
        return;

    // set prune
    switch (checkBoxPrune->checkState()) {
    case Qt::Unchecked:
        mFetchOptions->setPrune(Git::FetchOptions::Prune::NoPrune);
        break;
    case Qt::PartiallyChecked:
        mFetchOptions->setPrune(Git::FetchOptions::Prune::Unspecified);
        break;
    case Qt::Checked:
        mFetchOptions->setPrune(Git::FetchOptions::Prune::True);
        break;
    }

    // set download tags
    switch (checkBoxTags->checkState()) {
    case Qt::Unchecked:
        mFetchOptions->setDownloadTags(Git::FetchOptions::DownloadTags::None);
        break;
    case Qt::PartiallyChecked:
        mFetchOptions->setDownloadTags(Git::FetchOptions::DownloadTags::Auto);
        break;
    case Qt::Checked:
        mFetchOptions->setDownloadTags(Git::FetchOptions::DownloadTags::All);
        break;
    }

    // set redirect
    switch (checkBoxRedirect->checkState()) {
    case Qt::Unchecked:
        mFetchOptions->setRedirect(Git::Redirect::None);
        break;
    case Qt::PartiallyChecked:
        mFetchOptions->setRedirect(Git::Redirect::Initial);
        break;
    case Qt::Checked:
        mFetchOptions->setRedirect(Git::Redirect::All);
        break;
    }

    // set depth
    if (checkBoxDepth->isChecked())
        mFetchOptions->setDepth(spinBoxDepth->value());

    mIsChanged = false;
    mRetryCount = 0;
    // auto success = mGit->fetch(remote, mFetch);

    QThreadPool::globalInstance()->start([=]() {
        auto ok = mGit->fetch(remote, Git::Branch{}, mFetchOptions);

        metaObject()->invokeMethod(this, "slotFetchFinished", Q_ARG(bool, ok));
    });
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
    buttonBox_2->button(QDialogButtonBox::Close)->setEnabled(true);

    if (success) {
        labelStatus->setText(i18n("Finished"));
    } else {
        labelStatus->setText(i18n("Finished with error"));
        textBrowser->append(i18n("Error %1: %2", Git::Error::lastType(), Git::Error::lastMessage()));
    }

    progressBar->setValue(progressBar->maximum());
}

void FetchDialog::slotCredentialRequested(const QString &url, Git::Credential *cred, bool *accept)
{
    CredentialDialog d{this};
    d.setUrl(url);

    if (d.exec() == QDialog::Accepted) {
        cred->setUsername(d.username());
        cred->setPassword(d.password());
        *accept = true;
    } else {
        *accept = false;
    }
}

void FetchDialog::slotCertificateCheck(const Git::Certificate &cert, bool *accept)
{
    if (cert.isValid()) {
        *accept = true;
        return;
    }
    if (++mRetryCount > 3) {
        *accept = false;
        return;
    }
    CertificateInfoDialog d{cert, this};
    *accept = d.exec() == QDialog::Accepted;
}

#include "moc_fetchdialog.cpp"
