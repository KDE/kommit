/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "clonedialog.h"

#include <KLocalizedString>
#include <QSettings>
#include <QStandardPaths>

CloneDialog::CloneDialog(QWidget *parent)
    : AppDialog(parent)
    , mFixedPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
{
    setupUi(this);
    loadSettings();
}

CloneDialog::~CloneDialog() = default;

void CloneDialog::loadSettings()
{
    QSettings s;
    lineEditUrl->setText(s.value(QStringLiteral("lastClonedRepo")).toString());
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CloneDialog::slotAccepted);
    connect(lineEditUrl, &QLineEdit::textChanged, this, &CloneDialog::slotUrlChanged);
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
    slotUrlChanged(path);
}

void CloneDialog::slotUrlChanged(const QString &text)
{
    const auto parts = text.split(QLatin1Char('/'));
    if (!parts.isEmpty()) {
        auto local = parts.last();
        if (local.endsWith(QStringLiteral(".git"), Qt::CaseInsensitive)) {
            local = local.mid(0, local.size() - 4);
            lineEditPath->setText(mFixedPath + QLatin1Char('/') + local);
        }
    }
}

void CloneDialog::slotAccepted()
{
    QSettings s;
    s.setValue(QStringLiteral("lastClonedRepo"), lineEditUrl->text());

    accept();
}
