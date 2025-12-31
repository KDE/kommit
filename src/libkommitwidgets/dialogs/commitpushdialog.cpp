/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitpushdialog.h"

#include "actions/changedfileactions.h"
#include "certificateinfodialog.h"
#include "changedsubmodulesdialog.h"
#include "commands/commandcommit.h"
#include "commands/commandpush.h"
#include "credentialdialog.h"
#include "dialogs/changedsubmodulesdialog.h"
#include "models/changedfilesmodel.h"
#include "runnerdialog.h"

#include <Kommit/BranchesCache>
#include <Kommit/CommitsCache>
#include <Kommit/Error>
#include <Kommit/Index>
#include <Kommit/PushOptions>
#include <Kommit/RemoteCallbacks>
#include <Kommit/RemotesCache>
#include <Kommit/Repository>
#include <Kommit/SubModule>
#include <Kommit/SubmodulesCache>

#include <QWindow>
#include <QThreadPool>

#include <KSharedConfig>
#include <KWindowConfig>

namespace
{
static const char myCommitPushDialogGroupName[] = "CommitPushDialog";
}

CommitPushDialog::CommitPushDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mChangedFilesModel(new ChangedFilesModel(git, true, this))
    , mPushOptions{git}
{
    setupUi(this);

    reload();

    mActions = new ChangedFileActions(mGit, this);

    checkBoxIncludeStatus->setCheckState(Qt::PartiallyChecked);

    connect(pushButtonCommit, &QPushButton::clicked, this, &CommitPushDialog::slotPushButtonCommitClicked);
    connect(pushButtonPush, &QPushButton::clicked, this, &CommitPushDialog::slotPushButtonPushClicked);
    connect(toolButtonAddAll, &QToolButton::clicked, this, &CommitPushDialog::slotToolButtonAddAllClicked);
    connect(toolButtonAddNone, &QToolButton::clicked, this, &CommitPushDialog::slotToolButtonAddNoneClicked);
    connect(toolButtonAddIndexed, &QToolButton::clicked, this, &CommitPushDialog::slotToolButtonAddIndexedClicked);
    connect(toolButtonAddAdded, &QToolButton::clicked, this, &CommitPushDialog::slotToolButtonAddAddedClicked);
    connect(toolButtonAddModified, &QToolButton::clicked, this, &CommitPushDialog::slotToolButtonAddModifiedClicked);
    connect(toolButtonAddRemoved, &QToolButton::clicked, this, &CommitPushDialog::slotToolButtonAddRemovedClicked);
    connect(listView, &QListView::doubleClicked, this, &CommitPushDialog::slotListWidgetItemDoubleClicked);
    connect(listView, &QListView::customContextMenuRequested, this, &CommitPushDialog::slotListWidgetCustomContextMenuRequested);
    connect(groupBoxMakeCommit, &QGroupBox::toggled, this, &CommitPushDialog::slotGroupBoxMakeCommitToggled);
    connect(mChangedFilesModel, &ChangedFilesModel::checkedCountChanged, this, &CommitPushDialog::checkButtonsEnable);
    connect(textEditMessage, &QTextEdit::textChanged, this, &CommitPushDialog::checkButtonsEnable);
    connect(checkBoxAmend, &QCheckBox::toggled, this, &CommitPushDialog::checkButtonsEnable);
    connect(mActions, &ChangedFileActions::reloadNeeded, mChangedFilesModel, &ChangedFilesModel::reload);

    auto callbacks = mPushOptions.remoteCallbacks();
    connect(callbacks, &Git::RemoteCallbacks::message, this, &CommitPushDialog::slotPushMessage);
    connect(callbacks, &Git::RemoteCallbacks::transferProgress, this, &CommitPushDialog::slotPushTransferProgress);
    connect(callbacks, &Git::RemoteCallbacks::packProgress, this, &CommitPushDialog::slotPushPackProgress);
    connect(callbacks, &Git::RemoteCallbacks::updateRef, this, &CommitPushDialog::slotPushUpdateRef);
    connect(callbacks, &Git::RemoteCallbacks::credentialRequested, this, &CommitPushDialog::slotCredentialRequested, Qt::BlockingQueuedConnection);
    connect(callbacks, &Git::RemoteCallbacks::certificateCheck, this, &CommitPushDialog::slotCertificateCheck, Qt::BlockingQueuedConnection);

    listView->setModel(mChangedFilesModel);
    mChangedFilesModel->reload();
    readConfig();
    stackedWidget->setCurrentIndex(0);

    auto submodules = mGit->submodules()->allSubmodules();
    auto modifiedSubmoduleFound = std::any_of(submodules.constBegin(), submodules.constEnd(), [](const Git::Submodule &submodule) {
        return submodule.status() & Git::Submodule::Status::WdWdModified;
    });

    if (modifiedSubmoduleFound) {
        ChangedSubmodulesDialog d{git};
        d.exec();
        reload();
    }

}

CommitPushDialog::~CommitPushDialog()
{
    writeConfig();
}

void CommitPushDialog::slotPushMessage(const QString &message)
{
    labelStatus->setText(message);
}

void CommitPushDialog::slotPushTransferProgress(const Git::FetchTransferStat *stat)
{
    labelStatus->setText(i18n("Sending objects"));
    progressBar->setMaximum(stat->totalObjects);
    progressBar->setValue(stat->receivedObjects);
}

void CommitPushDialog::slotPushPackProgress(const Git::PackProgress *p)
{
    labelStatus->setText(i18n("Stage %1", p->stage));
    progressBar->setMaximum(p->total);
    progressBar->setValue(p->current);
}

void CommitPushDialog::slotPushUpdateRef(const Git::Reference &reference, const Git::Oid &a, const Git::Oid &b)
{
    mIsChanged = true;
    textBrowser->append(i18n("Reference updated: %1  <a href=\"%2\">%2</a>...<a href=\"%3\">%3</a>", reference.name(), a.toString(), b.toString()));
}

void CommitPushDialog::slotFetchFinished(bool success)
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

void CommitPushDialog::slotCredentialRequested(const QString &url, Git::Credential *cred, bool *accept)
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

void CommitPushDialog::slotCertificateCheck(const Git::Certificate &cert, bool *accept)
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

void CommitPushDialog::readConfig()
{
    create(); // ensure a window is created
    windowHandle()->resize(QSize(800, 300));
    KConfigGroup group(KSharedConfig::openStateConfig(), myCommitPushDialogGroupName);
    KWindowConfig::restoreWindowSize(windowHandle(), group);
    resize(windowHandle()->size()); // workaround for QTBUG-40584
}

void CommitPushDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myCommitPushDialogGroupName);
    KWindowConfig::saveWindowSize(windowHandle(), group);
}

void CommitPushDialog::reload()
{
    mChangedFilesModel->reload();

    if (!mChangedFilesModel->size()) {
        pushButtonCommit->setEnabled(false);
        pushButtonPush->setEnabled(true);
        groupBoxMakeCommit->setEnabled(false);
        pushButtonPush->setText(i18n("Push"));
    } else {
        pushButtonPush->setText(i18n("Commit and push"));
    }

    comboBoxBranch->clear();
    comboBoxRemote->clear();
    auto branches = mGit->branches()->names(Git::BranchType::LocalBranch);
    auto remotes = mGit->remotes()->allNames();
    comboBoxBranch->addItems(branches);
    comboBoxRemote->addItems(remotes);
    labelCurrentBranchName->setText(mGit->branches()->currentName());

    QSet<QString> _words;
    for (const auto &b : std::as_const(branches))
        _words.insert(b);
    for (const auto &r : std::as_const(remotes))
        _words.insert(r);
    auto data = mChangedFilesModel->data();
    for (const auto &row : data) {
        const auto parts = row.filePath.split(QLatin1Char('/'));
        for (const auto &p : parts)
            _words.insert(p);
        _words.insert(row.filePath);
    }
    textEditMessage->addWords(_words.values());
    textEditMessage->begin();
}

void CommitPushDialog::checkButtonsEnable()
{
    if (groupBoxMakeCommit->isEnabled() && !groupBoxMakeCommit->isChecked()) {
        pushButtonCommit->setEnabled(false);
        pushButtonPush->setEnabled(true);
        return;
    }
    bool enable{false};

    if (!mChangedFilesModel->checkedCount()) {
        pushButtonCommit->setEnabled(false);
        pushButtonPush->setEnabled(false);
        return;
    }

    enable = checkBoxAmend->isChecked() || !textEditMessage->toPlainText().isEmpty();
    pushButtonCommit->setEnabled(enable);
    pushButtonPush->setEnabled(enable);
}

void CommitPushDialog::slotPushButtonCommitClicked()
{
    addFiles();

    // Git::CommandCommit *cmd = new Git::CommandCommit;
    // cmd->setAmend(checkBoxAmend->isChecked());
    // cmd->setMessage(textEditMessage->toPlainText());
    // cmd->setIncludeStatus(Git::checkStateToOptionalBool(checkBoxIncludeStatus->checkState()));

    // RunnerDialog d(mGit);
    // d.run(cmd);
    // d.exec();
    commit();
    accept();
}

void CommitPushDialog::slotPushButtonPushClicked()
{
    if (groupBoxMakeCommit->isChecked()) {
        addFiles();
        commit();
        // Git::CommandCommit *commitCommand = new Git::CommandCommit;
        // commitCommand->setAmend(checkBoxAmend->isChecked());
        // commitCommand->setMessage(textEditMessage->toPlainText());
        // commitCommand->setIncludeStatus(Git::checkStateToOptionalBool(checkBoxIncludeStatus->checkState()));

        // RunnerDialog d(mGit, this);
        // d.setAutoClose(true);
        // d.run(commitCommand);
        // auto dd = d.exec();
        // //        qDebug() << dd;
        // if (dd != QDialog::Accepted)
        //     return;

        // if (!mGit->commits()->create(textEditMessage->toPlainText())) {
        //     // TODO: show messagebox
        //     return;
        // }
    }

    auto branch = mGit->branches()->findByName(comboBoxBranch->currentText());
    auto remote = mGit->remotes()->findByName(comboBoxRemote->currentText());

    mPushOptions.setBranch(branch);
    mPushOptions.setRemote(remote);

    QThreadPool::globalInstance()->start([=]() {
        mGit->push(branch, remote, &mPushOptions);

        // metaObject()->invokeMethod(this, "slotFetchFinished", Q_ARG(bool, ok));
    });
    stackedWidget->setCurrentIndex(1);

    // Git::CommandPush *cmd = new Git::CommandPush;
    // cmd->setRemote(comboBoxRemote->currentText());

    // if (radioButtonCurrentBranch->isChecked())
    //     cmd->setLocalBranch(labelCurrentBranchName->text());
    // else if (radioButtonExistingBranch->isChecked())
    //     cmd->setLocalBranch(comboBoxBranch->currentText());
    // else
    //     cmd->setLocalBranch(lineEditNewBranchName->text());
    // cmd->setForce(checkBoxForce->isChecked());

    // mGit->commit(textEditMessage->toPlainText());
    // RunnerDialog d(mGit, this);
    // d.run(cmd);
    // d.exec();
    // accept();
}

void CommitPushDialog::addFiles()
{
    auto index = mGit->index();
    for (const auto &file : mChangedFilesModel->data()) {
        if (!file.checked)
            continue;
        if (file.status == Git::ChangeStatus::Removed)
            Q_UNUSED(index.removeByPath(file.filePath))
        else
            Q_UNUSED(index.addByPath(file.filePath))
    }
    index.write();
}

void CommitPushDialog::commit()
{
    Git::CommitOptions options;

    if (checkBoxAmend->isChecked())
        mGit->commits()->amend(textEditMessage->toPlainText(), options);
    else
        mGit->commits()->create(textEditMessage->toPlainText(), options);
}

void CommitPushDialog::slotToolButtonAddAllClicked()
{
    mChangedFilesModel->toggleCheckAll(true);
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddNoneClicked()
{
    mChangedFilesModel->toggleCheckAll(false);
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddIndexedClicked()
{
    mChangedFilesModel->checkByStatus({Git::ChangeStatus::Removed, Git::ChangeStatus::Modified});
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddAddedClicked()
{
    mChangedFilesModel->checkByStatus(Git::ChangeStatus::Untracked);
}

void CommitPushDialog::slotToolButtonAddRemovedClicked()
{
    mChangedFilesModel->checkByStatus(Git::ChangeStatus::Removed);
}

void CommitPushDialog::slotToolButtonAddModifiedClicked()
{
    mChangedFilesModel->checkByStatus(Git::ChangeStatus::Modified);
}

void CommitPushDialog::slotListWidgetItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto data = mChangedFilesModel->data(index.row());

    if (!data->submodule.isNull())
        return;

    if (data->oldFilePath != QString())
        mActions->setFilePaths(data->oldFilePath, data->filePath);
    else
        mActions->setFilePath(data->filePath);
    mActions->diff();
}

void CommitPushDialog::slotGroupBoxMakeCommitToggled(bool checked)
{
    checkButtonsEnable();
    pushButtonPush->setText(checked ? i18n("Commit and push") : i18n("Push"));
}

void CommitPushDialog::slotListWidgetCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (!listView->currentIndex().isValid())
        return;

    mActions->setFilePath(mChangedFilesModel->filePath(listView->currentIndex().row()));
    mActions->popup();
}

#include "moc_commitpushdialog.cpp"
