/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "commitpushdialog.h"
#include "GitKlientSettings.h"
#include "actions/changedfileactions.h"
#include "commands/commandcommit.h"
#include "commands/commandpush.h"
#include "gitmanager.h"
#include "runnerdialog.h"

#include <QPainter>

// TODO: improve this method (Add cache)
QIcon createIcon(const QColor &color)
{
    QPixmap pixmap(32, 32);
    QPainter p(&pixmap);
    p.setBrush(color);
    p.setPen(color);
    p.fillRect(0, 0, 32, 32, Qt::color1);

    p.setPen(Qt::black);
    p.drawEllipse({16, 16}, 8, 8);

    QImage image = pixmap.toImage();
    image.setAlphaChannel(pixmap.toImage());

    return QIcon(QPixmap::fromImage(image));
}

void CommitPushDialog::reload()
{
    auto files = mGit->changedFiles();

    QMap<Git::Manager::ChangeStatus, QIcon> icons;

    for (auto i = files.constBegin(); i != files.constEnd(); ++i) {
        auto item = new QListWidgetItem(listWidget);
        item->setText(i.key());

        if (icons.contains(i.value())) {
            item->setIcon(icons.value(i.value()));
        } else {
            QColor cl;
            switch (i.value()) {
            case Git::Manager::Modified:
                cl = GitKlientSettings::diffModifiedColor();
                break;
            case Git::Manager::Added:
            case Git::Manager::Untracked:
                cl = GitKlientSettings::diffAddedColor();
                break;
            case Git::Manager::Removed:
                cl = GitKlientSettings::diffRemovedColor();
                break;

            default:
                cl = Qt::yellow;
                qWarning() << "File status" << i.value() << "is not implemented";
                break;
            }
            auto icon = createIcon(cl);
            icons.insert(i.value(), icon);
            item->setIcon(icon);
        }
        item->setCheckState(Qt::Unchecked);
        item->setData(StatusRole, (int)i.value());
        listWidget->addItem(item);
    }
    if (files.empty()) {
        pushButtonCommit->setEnabled(false);
        pushButtonPush->setEnabled(true);
        groupBoxMakeCommit->setEnabled(false);
        pushButtonPush->setText(i18n("Push"));
    } else {
        pushButtonPush->setText(i18n("Commit and push"));
    }

    comboBoxBranch->clear();
    comboBoxRemote->clear();
    auto branches = mGit->branches();
    auto remotes = mGit->remotes();
    comboBoxBranch->addItems(branches);
    comboBoxRemote->addItems(remotes);
    labelCurrentBranchName->setText(mGit->currentBranch());

    QSet<QString> _words;
    for (const auto &b : std::as_const(branches))
        _words.insert(b);
    for (const auto &r : std::as_const(remotes))
        _words.insert(r);
    for (auto i = files.constBegin(); i != files.constEnd(); ++i) {
        const auto parts = i.key().split(QLatin1Char('/'));
        for (const auto &p : parts)
            _words.insert(p);
        _words.insert(i.key());
    }
    textEditMessage->addWords(_words.values());
    textEditMessage->begin();
}

void CommitPushDialog::checkItemsByStatus(int status)
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        auto itemStatus = item->data(StatusRole).toInt();
        item->setCheckState(status == itemStatus ? Qt::Checked : Qt::Unchecked);
    }
}

CommitPushDialog::CommitPushDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
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
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &CommitPushDialog::slotListWidgetItemDoubleClicked);
    connect(listWidget, &QListWidget::itemClicked, this, &CommitPushDialog::slotListWidgetItemClicked);
    connect(groupBoxMakeCommit, &QGroupBox::toggled, this, &CommitPushDialog::slotGroupBoxMakeCommitToggled);
    connect(listWidget, &QListWidget::customContextMenuRequested, this, &CommitPushDialog::slotListWidgetCustomContextMenuRequested);
}

void CommitPushDialog::checkButtonsEnable()
{
    if (groupBoxMakeCommit->isEnabled() && !groupBoxMakeCommit->isChecked()) {
        pushButtonCommit->setEnabled(false);
        pushButtonPush->setEnabled(true);
        return;
    }
    bool enable{false};

    for (auto i = 0; i < listWidget->count(); ++i) {
        if (listWidget->item(i)->checkState() == Qt::Checked) {
            enable = true;
            break;
        }
    }

    if (!enable) {
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
        qDebug() << dd;
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

void CommitPushDialog::slotToolButtonAddAllClicked()
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
    checkButtonsEnable();
}

void CommitPushDialog::addFiles()
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        if (item->checkState() == Qt::Checked)
            mGit->addFile(item->text());
    }
}

void CommitPushDialog::slotToolButtonAddNoneClicked()
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddIndexedClicked()
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
    checkButtonsEnable();
}

void CommitPushDialog::slotToolButtonAddAddedClicked()
{
    checkItemsByStatus(Git::Manager::Untracked);
}

void CommitPushDialog::slotToolButtonAddRemovedClicked()
{
    checkItemsByStatus(Git::Manager::Removed);
}

void CommitPushDialog::slotToolButtonAddModifiedClicked()
{
    checkItemsByStatus(Git::Manager::Modified);
}

void CommitPushDialog::slotListWidgetItemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->diff();
}
void CommitPushDialog::slotListWidgetItemClicked(QListWidgetItem *)
{
    checkButtonsEnable();
}

void CommitPushDialog::slotGroupBoxMakeCommitToggled(bool checked)
{
    checkButtonsEnable();
    pushButtonPush->setText(checked ? i18n("Commit and push") : i18n("Push"));
}

void CommitPushDialog::slotListWidgetCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (listWidget->currentRow() == -1)
        return;

    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->popup();
}
