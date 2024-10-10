/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "reposettingsdialog.h"

#include "repository.h"

RepoSettingsDialog::RepoSettingsDialog(Git::Repository *git, QWidget *parent)
    : AppDialog(git, parent)
{
    setupUi(this);

    lineEditUserName->setText(git->config(QStringLiteral("user.name")));
    lineEditUserEmail->setText(git->config(QStringLiteral("user.email")));

    connect(buttonBox, &QDialogButtonBox::accepted, this, &RepoSettingsDialog::slotAccepted);

    initComboBox<AutoCrlf>(comboBoxAutoCrlf);
    initComboBox<FileMode>(comboBoxFileMode);

    auto autoCrlf = git->config(QStringLiteral("core.autocrlf"));
    auto fileMode = git->config(QStringLiteral("core.fileMode"));

    if (autoCrlf == QStringLiteral("input"))
        setComboboxValue(comboBoxAutoCrlf, AutoCrlf::Input);
    else if (autoCrlf == QStringLiteral("true") || autoCrlf == QStringLiteral("yes") || autoCrlf == QStringLiteral("on"))
        setComboboxValue(comboBoxAutoCrlf, AutoCrlf::Enable);
    else if (autoCrlf == QStringLiteral("false") || autoCrlf == QStringLiteral("no") || autoCrlf == QStringLiteral("off"))
        setComboboxValue(comboBoxAutoCrlf, AutoCrlf::Disable);
    else
        setComboboxValue(comboBoxAutoCrlf, AutoCrlf::Unset);

    if (fileMode == QStringLiteral("true") || fileMode == QStringLiteral("yes") || fileMode == QStringLiteral("on"))
        setComboboxValue(comboBoxFileMode, FileMode::Enable);
    else if (fileMode == QStringLiteral("false") || fileMode == QStringLiteral("no") || fileMode == QStringLiteral("off"))
        setComboboxValue(comboBoxFileMode, FileMode::Disable);
    else
        setComboboxValue(comboBoxFileMode, FileMode::Unset);
}

void RepoSettingsDialog::slotAccepted()
{
    mGit->setConfig(QStringLiteral("user.name"), lineEditUserName->text());
    mGit->setConfig(QStringLiteral("user.email"), lineEditUserEmail->text());

    auto autoCrlf = comboBoxCurrentValue<AutoCrlf>(comboBoxAutoCrlf);
    auto fileMode = comboBoxCurrentValue<FileMode>(comboBoxFileMode);

    switch (autoCrlf) {
    case AutoCrlf::Unset:
        mGit->unsetConfig(QStringLiteral("core.autocrlf"));
        break;
    case AutoCrlf::Enable:
        mGit->setConfig(QStringLiteral("core.autocrlf"), "true");
        break;
    case AutoCrlf::Disable:
        mGit->setConfig(QStringLiteral("core.autocrlf"), "false");
        break;
    case AutoCrlf::Input:
        mGit->setConfig(QStringLiteral("core.autocrlf"), "input");
        break;
    }

    switch (fileMode) {
    case FileMode::Unset:
        mGit->unsetConfig(QStringLiteral("core.fileMode"));
        break;
    case FileMode::Enable:
        mGit->setConfig(QStringLiteral("core.fileMode"), "true");
        break;
    case FileMode::Disable:
        mGit->setConfig(QStringLiteral("core.fileMode"), "false");
        break;
    }

    accept();
}

#include "moc_reposettingsdialog.cpp"
