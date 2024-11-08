/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clonedialog.h"

#include <KLineEdit>
#include <KLocalizedString>
#include <QMessageBox>
#include <QPointer>
#include <QSettings>
#include <QStandardPaths>

#include <Kommit/CheckoutOptions>
#include <Kommit/Clone>
#include <Kommit/CloneObserver>
#include <Kommit/CloneOptions>
#include <Kommit/Credential>
#include <Kommit/Fetch>
#include <Kommit/FetchObserver>

#include <dialogs/credentialdialog.h>
#include <repository.h>

CloneDialog::CloneDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
    , mFixedPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
    , mCloneOptions{new Git::CloneOptions{git}}
{
    setupUi(this);
    loadSettings();
    lineEditPath->setMode(UrlRequester::Mode::Directory);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CloneDialog::slotAccepted);
    connect(lineEditUrl, &QLineEdit::textChanged, this, &CloneDialog::slotUrlChanged);

    connect(lineEditPath->lineEdit(), &KLineEdit::textChanged, this, &CloneDialog::slotUrlChanged);

    stackedWidget->setCurrentIndex(0);
    updateOkButton();

    auto checkoutOptions = mCloneOptions->checkoutOptions();

    connect(checkoutOptions, &Git::CheckoutOptions::checkoutNotify, this, &CloneDialog::slotCheckoutNotify);
    connect(checkoutOptions, &Git::CheckoutOptions::checkoutProgress, this, &CloneDialog::slotCheckoutProgress);
    connect(checkoutOptions, &Git::CheckoutOptions::checkoutPerfData, this, &CloneDialog::slotCheckoutPerfData);
}

CloneDialog::~CloneDialog()
{
    delete mCloneOptions;
}

void CloneDialog::updateOkButton()
{
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!lineEditUrl->text().trimmed().isEmpty() && !lineEditPath->text().trimmed().isEmpty());
}

void CloneDialog::loadSettings()
{
    QSettings s;
    lineEditUrl->setText(s.value(QStringLiteral("lastClonedRepo")).toString());
}

Git::CloneCommand *CloneDialog::command()
{
    auto cmd = new Git::CloneCommand(this);

    cmd->setRepoUrl(lineEditUrl->text());
    cmd->setLocalPath(lineEditPath->text());
    if (checkBoxBranch->isChecked())
        cmd->setBranch(lineEditBranch->text());
    if (checkBoxDepth->isChecked())
        cmd->setDepth(spinBoxDepth->value());
    cmd->setRecursive(checkBoxRecursive->isChecked());
    return cmd;
}

void CloneDialog::setLocalPath(const QString &path)
{
    if (path.endsWith(QLatin1Char('/')))
        mFixedPath = path.mid(0, path.length() - 1);
    else
        mFixedPath = path;
    lineEditPath->setText(path);
    slotUrlChanged(lineEditUrl->text());
}

void CloneDialog::slotCheckoutNotify(Git::CheckoutOptions::NotifyFlag notify, QString path, Git::DiffFile baseline, Git::DiffFile target, Git::DiffFile workdir)
{
}

void CloneDialog::slotCheckoutProgress(QString path, size_t completed_steps, size_t total_steps)
{
    labelMessage->setText(path);
    progressBar->setMaximum(total_steps);
    progressBar->setValue(completed_steps);
}

void CloneDialog::slotCheckoutPerfData(size_t mkdir_calls, size_t stat_calls, size_t chmod_calls)
{
}

void CloneDialog::performClone()
{
    stackedWidget->setCurrentIndex(1);
}

void CloneDialog::slotCredentialRequeted(const QString &url, Git::Credential *cred)
{
    CredentialDialog d;
    d.setUsername(cred->username());
    d.setUrl(url);
    if (d.exec() == QDialog::Accepted) {
        cred->setUsername(d.username());
        cred->setPassword(d.password());
    }
}

void CloneDialog::slotUrlChanged(const QString &text)
{
    updateOkButton();
    if (text.isEmpty()) {
        lineEditPath->clear();
        return;
    }
    const auto parts = text.split(QLatin1Char('/'));
    if (!parts.isEmpty()) {
        auto local = parts.last();
        if (local.endsWith(QStringLiteral(".git"), Qt::CaseInsensitive))
            local = local.mid(0, local.size() - 4);
        else
            local = local.replace(QStringLiteral("."), QStringLiteral("_"));
        lineEditPath->setText(mFixedPath + QLatin1Char('/') + local);
    }
}

void CloneDialog::slotAccepted()
{
    QSettings s;
    s.setValue(QStringLiteral("lastClonedRepo"), lineEditUrl->text());

    accept();

    // stackedWidget->setCurrentIndex(1);
    // QPointer<Git::Manager> git{new Git::Manager};

    // auto ok = git->clone(lineEditUrl->text(), lineEditPath->text(), mCloneObserver);
    // if (!ok)
    //     QMessageBox::warning(this, i18n("Clone"), git->errorMessage());
}

#include "moc_clonedialog.cpp"
