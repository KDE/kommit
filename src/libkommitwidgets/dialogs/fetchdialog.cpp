/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fetchdialog.h"

#include <Kommit/BranchesCache>
#include <Kommit/CommandFetch>
#include <Kommit/Credential>
#include <Kommit/Fetch>
#include <Kommit/FetchObserver>
#include <Kommit/Oid>
#include <Kommit/Reference>
#include <Kommit/RemotesCache>
#include <Kommit/Repository>

#include "credentialdialog.h"
#include "runnerdialog.h"
#include "certificateinfodialog.h"

#include <QDialogButtonBox>

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
    Git::Fetch fetch{mGit};

    auto remote = mGit->remotes()->findByName(comboBoxRemote->currentText());
    fetch.setRemote(remote);

    // set prune
    switch (checkBoxPrune->checkState()) {
    case Qt::Unchecked:
        fetch.setPrune(Git::Fetch::Prune::NoPrune);
        break;
    case Qt::PartiallyChecked:
        fetch.setPrune(Git::Fetch::Prune::PruneUnspecified);
        break;
    case Qt::Checked:
        fetch.setPrune(Git::Fetch::Prune::Prune);
        break;
    }

    // set download tags
    switch (checkBoxTags->checkState()) {
    case Qt::Unchecked:
        fetch.setDownloadTags(Git::Fetch::DownloadTags::None);
        break;
    case Qt::PartiallyChecked:
        fetch.setDownloadTags(Git::Fetch::DownloadTags::Auto);
        break;
    case Qt::Checked:
        fetch.setDownloadTags(Git::Fetch::DownloadTags::All);
        break;
    }

    // set redirect
    switch (checkBoxRedirect->checkState()) {
    case Qt::Unchecked:
        fetch.setRedirect(Git::Fetch::Redirect::None);
        break;
    case Qt::PartiallyChecked:
        fetch.setRedirect(Git::Fetch::Redirect::Initial);
        break;
    case Qt::Checked:
        fetch.setRedirect(Git::Fetch::Redirect::All);
        break;
    }

    // set depth
    if (checkBoxDepth->isChecked())
        fetch.setDepth(spinBoxDepth->value());

    connect(&fetch, &Git::Fetch::message, this, &FetchDialog::slotFetchMessage);
    connect(&fetch, &Git::Fetch::credentialRequeted, this, &FetchDialog::slotFetchCredentialRequeted);
    connect(&fetch, &Git::Fetch::transferProgress, this, &FetchDialog::slotFetchTransferProgress);
    connect(&fetch, &Git::Fetch::packProgress, this, &FetchDialog::slotFetchPackProgress);
    connect(&fetch, &Git::Fetch::updateRef, this, &FetchDialog::slotFetchUpdateRef);
    connect(&fetch, &Git::Fetch::certificateCheck, this, &FetchDialog::slotFetchCertificateCheck);

    stackedWidget->setCurrentIndex(1);
    fetch.run();
}

void FetchDialog::slotFetchMessage(const QString &message)
{
    textBrowser->append(message);
}

void FetchDialog::slotFetchCredentialRequeted(const QString &url, Git::Credential *cred)
{
    CredentialDialog d{this};
    d.setUrl(url);

    if (d.exec() == QDialog::Accepted) {
        cred->setUsername(d.username());
        cred->setPassword(d.password());
    }
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

void FetchDialog::slotFetchUpdateRef(QSharedPointer<Git::Reference> reference, QSharedPointer<Git::Oid> a, QSharedPointer<Git::Oid> b)
{
    textBrowser->append(i18n("Reference updated: %1  %2...%3", reference->name(), a->toString(), b->toString()));
}

void FetchDialog::slotFetchCertificateCheck(Git::Certificate *cert, bool *accept)
{
    Q_UNUSED(cert)

    CertificateInfoDialog d{cert, this};
    *accept = d.exec() == QDialog::Accepted;
}

#include "moc_fetchdialog.cpp"
