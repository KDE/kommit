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

#include <KTextEditor/Editor>

// TODO: improve this method (Add cache)
QIcon createIcon(const QColor &color)
{
    QPixmap pixmap(32, 32);
    QPainter p(&pixmap);
    p.setBrush(color);
    p.setPen(color);
    p.fillRect(0, 0, 32, 32, Qt::color1);
    p.drawEllipse({16, 16}, 8, 8);

    QImage image = pixmap.toImage();
    image.setAlphaChannel(pixmap.toImage());

    return QIcon(QPixmap::fromImage(image));
}
CommitPushDialog::CommitPushDialog(Git::Manager *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    const auto files = git->changedFiles();
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
                cl = GitKlientSettings::diffAddedColor();
                break;
            case Git::Manager::Removed:
                cl = GitKlientSettings::diffRemovedColor();
                break;

            default:
                break;
            }
            auto icon = createIcon(cl);
            icons.insert(i.value(), icon);
            item->setIcon(icon);
        }
        item->setCheckState(Qt::Unchecked);
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

    auto branches = git->branches();
    auto remotes = git->remotes();
    comboBoxBranch->addItems(branches);
    comboBoxRemote->addItems(remotes);
    labelCurrentBranchName->setText(git->currentBranch());

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

    mActions = new ChangedFileActions(mGit, this);

    checkBoxIncludeStatus->setCheckState(Qt::PartiallyChecked);
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

void CommitPushDialog::on_pushButtonCommit_clicked()
{
    addFiles();
    Git::CommandCommit cmd;
    cmd.setAmend(checkBoxAmend->isChecked());
    cmd.setMessage(textEditMessage->toPlainText());
    cmd.setIncludeStatus(Git::checkStateToOptionalBool(checkBoxIncludeStatus->checkState()));

    mGit->run(cmd);
    accept();
}

void CommitPushDialog::on_pushButtonPush_clicked()
{
    addFiles();

    if (groupBoxMakeCommit->isChecked()) {
        addFiles();
        Git::CommandCommit commitCommand;
        commitCommand.setAmend(checkBoxAmend->isChecked());
        commitCommand.setMessage(textEditMessage->toPlainText());
        commitCommand.setIncludeStatus(Git::checkStateToOptionalBool(checkBoxIncludeStatus->checkState()));
        mGit->run(commitCommand);
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
    RunnerDialog d(this);
    d.run(&cmd);
    d.exec();
    accept();
}

void CommitPushDialog::on_toolButtonAddAll_clicked()
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

void CommitPushDialog::on_toolButtonAddNone_clicked()
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        item->setCheckState(Qt::Unchecked);
    }
    checkButtonsEnable();
}

void CommitPushDialog::on_toolButtonAddIndexed_clicked()
{
    for (auto i = 0; i < listWidget->count(); ++i) {
        auto item = listWidget->item(i);
        item->setCheckState(Qt::Checked);
    }
    checkButtonsEnable();
}

void CommitPushDialog::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->diff();
}
void CommitPushDialog::on_listWidget_itemClicked(QListWidgetItem *)
{
    checkButtonsEnable();
}

void CommitPushDialog::on_groupBoxMakeCommit_toggled(bool checked)
{
    checkButtonsEnable();
    pushButtonPush->setText(checked ? i18n("Commit and push") : i18n("Push"));
}

void CommitPushDialog::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    if (listWidget->currentRow() == -1)
        return;

    mActions->setFilePath(listWidget->currentItem()->text());
    mActions->popup();
}
