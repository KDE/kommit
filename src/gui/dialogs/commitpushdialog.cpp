/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitpushdialog.h"
#include "actions/changedfileactions.h"
#include "commands/commandcommit.h"
#include "commands/commandpush.h"
#include "gitmanager.h"
#include "models/changedfilesmodel.h"
#include "runnerdialog.h"
#include <QPainter>

#include <KSharedConfig>
#include <KWindowConfig>
#include <QWindow>

namespace
{
static const char myCommitPushDialogGroupName[] = "CommitPushDialog";
}
CommitPushDialog::CommitPushDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
    , mModel(new ChangedFilesModel(git, true, this))
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
    connect(mModel, &ChangedFilesModel::checkedCountChanged, this, &CommitPushDialog::checkButtonsEnable);
    connect(textEditMessage, &QTextEdit::textChanged, this, &CommitPushDialog::checkButtonsEnable);
    connect(checkBoxAmend, &QCheckBox::toggled, this, &CommitPushDialog::checkButtonsEnable);
    connect(mActions, &ChangedFileActions::reloadNeeded, mModel, &ChangedFilesModel::reload);

    listView->setModel(mModel);
    mModel->reload();
    readConfig();
}

CommitPushDialog::~CommitPushDialog()
{
    writeConfig();
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
    mModel->reload();

    if (!mModel->size()) {
        pushButtonCommit->setEnabled(false);
        pushButtonPush->setEnabled(true);
        groupBoxMakeCommit->setEnabled(false);
        pushButtonPush->setText(i18n("Push"));
    } else {
        pushButtonPush->setText(i18n("Commit and push"));
    }

    comboBoxBranch->clear();
    comboBoxRemote->clear();
    auto branches = mGit->branches(Git::Manager::BranchType::LocalBranch);
    auto remotes = mGit->remotes();
    comboBoxBranch->addItems(branches);
    comboBoxRemote->addItems(remotes);
    labelCurrentBranchName->setText(mGit->currentBranch());

    QSet<QString> _words;
    for (const auto &b : std::as_const(branches))
        _words.insert(b);
    for (const auto &r : std::as_const(remotes))
        _words.insert(r);
    auto data = mModel->data();
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

    if (!mModel->checkedCount()) {
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
    Git::CommandCommit cmd;
    cmd.setAmend(checkBoxAmend->isChecked());
    cmd.setMessage(textEditMessage->toPlainText());
    cmd.setIncludeStatus(Git::checkStateToOptionalBool(checkBoxIncludeStatus->checkState()));

    RunnerDialog d(mGit);
    d.run(&cmd);
    d.exec();

    accept();
}

void CommitPushDialog::slotPushButtonPushClicked()
{
    addFiles();

    if (groupBoxMakeCommit->isChecked()) {
        addFiles();
        Git::CommandCommit commitCommand;
        commitCommand.setAmend(checkBoxAmend->isChecked());
        commitCommand.setMessage(textEditMessage->toPlainText());
        commitCommand.setIncludeStatus(Git::checkStateToOptionalBool(checkBoxIncludeStatus->checkState()));

        RunnerDialog d(mGit, this);
        d.setAutoClose(true);
        d.run(&commitCommand);
        auto dd = d.exec();
        //        qDebug() << dd;
        if (dd != QDialog::Accepted)
            return;
    }

    Git::CommandPush cmd;
    cmd.setRemote(comboBoxRemote->currentText());

    if (radioButtonCurrentBranch->isChecked())
        cmd.setLocalBranch(labelCurrentBranchName->text());
    else if (radioButtonExistingBranch->isChecked())
        cmd.setLocalBranch(comboBoxBranch->currentText());
    else
        cmd.setLocalBranch(lineEditNewBranchName->text());
    cmd.setForce(checkBoxForce->isChecked());

    mGit->commit(textEditMessage->toPlainText());
    RunnerDialog d(mGit, this);
    d.run(&cmd);
    d.exec();
    accept();
}

void CommitPushDialog::addFiles()
{
    auto files = mModel->checkedFiles();
    for (const auto &file : files) {
        mGit->addFile(file);
    }
}

void CommitPushDialog::slotToolButtonAddAllClicked()
{
    mModel->toggleCheckAll(true);
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddNoneClicked()
{
    mModel->toggleCheckAll(false);
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddIndexedClicked()
{
    mModel->checkByStatus({Git::ChangeStatus::Removed, Git::ChangeStatus::Modified});
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddAddedClicked()
{
    mModel->checkByStatus(Git::ChangeStatus::Untracked);
}

void CommitPushDialog::slotToolButtonAddRemovedClicked()
{
    mModel->checkByStatus(Git::ChangeStatus::Removed);
}

void CommitPushDialog::slotToolButtonAddModifiedClicked()
{
    mModel->checkByStatus(Git::ChangeStatus::Modified);
}

void CommitPushDialog::slotListWidgetItemDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    auto data = mModel->data(index.row());
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

    mActions->setFilePath(mModel->filePath(listView->currentIndex().row()));
    mActions->popup();
}

#include "moc_commitpushdialog.cpp"
