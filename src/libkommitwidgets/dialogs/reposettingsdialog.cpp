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

    auto config = git->config();

    lineEditUserName->setText(config.valueString(QStringLiteral("user.name")));
    lineEditUserEmail->setText(config.valueString(QStringLiteral("user.email")));

    connect(buttonBox, &QDialogButtonBox::accepted, this, &RepoSettingsDialog::slotAccepted);

    initComboBox<AutoCrlf>(comboBoxAutoCrlf);
    initComboBox<FileMode>(comboBoxFileMode);

    auto autoCrlf = config.valueString(QStringLiteral("core.autocrlf"));
    auto fileMode = config.valueString(QStringLiteral("core.fileMode"));

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
    auto config = mGit->config();
    config.set(QStringLiteral("user.name"), lineEditUserName->text());
    config.set(QStringLiteral("user.email"), lineEditUserEmail->text());

    auto autoCrlf = comboBoxCurrentValue<AutoCrlf>(comboBoxAutoCrlf);
    auto fileMode = comboBoxCurrentValue<FileMode>(comboBoxFileMode);

    switch (autoCrlf) {
    case AutoCrlf::Unset:
        config.remove(QStringLiteral("core.autocrlf"));
        break;
    case AutoCrlf::Enable:
        config.set(QStringLiteral("core.autocrlf"), "true");
        break;
    case AutoCrlf::Disable:
        config.set(QStringLiteral("core.autocrlf"), "false");
        break;
    case AutoCrlf::Input:
        config.set(QStringLiteral("core.autocrlf"), "input");
        break;
    }

    switch (fileMode) {
    case FileMode::Unset:
        config.remove(QStringLiteral("core.fileMode"));
        break;
    case FileMode::Enable:
        config.set(QStringLiteral("core.fileMode"), "true");
        break;
    case FileMode::Disable:
        config.set(QStringLiteral("core.fileMode"), "false");
        break;
    }

    accept();
}

#include "moc_reposettingsdialog.cpp"
