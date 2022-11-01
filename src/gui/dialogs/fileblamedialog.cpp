/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileblamedialog.h"

#include "gitmanager.h"
#include "models/logsmodel.h"

#include <KLocalizedString>

FileBlameDialog::FileBlameDialog(Git::Manager *git, const QString &fileName, QWidget *parent)
    : AppDialog(git, parent)
    , mFileName(fileName)
{
    setupUi(this);

    plainTextEdit->setHighlighting(fileName);
    const auto content = mGit->fileContent(QString(), mFileName);
    plainTextEdit->setPlainText(content);

    setWindowTitle(i18nc("@title:window", "Blame file: %1", fileName));
    logDetailsWidget->setEnableCommitsLinks(false);
    plainTextEdit->setShowTitleBar(false);

    connect(plainTextEdit, &BlameCodeView::blockSelected, this, &FileBlameDialog::slotPlainTextEditBlockSelected);
}

FileBlameDialog::FileBlameDialog(Git::Manager *git, const Git::File &file, QWidget *parent)
    : AppDialog(git, parent)
    , mFile(file)
{
    setupUi(this);
    plainTextEdit->setHighlighting(file.fileName());

    const auto b = file.git()->blame(file);
    plainTextEdit->setBlameData(b);

    setWindowTitle(i18nc("@title:window", "Blame file: %1", file.fileName()));

    logDetailsWidget->setEnableCommitsLinks(false);
    plainTextEdit->setShowTitleBar(false);

    connect(plainTextEdit, &BlameCodeView::blockSelected, this, &FileBlameDialog::slotPlainTextEditBlockSelected);
}

void FileBlameDialog::slotPlainTextEditBlockSelected()
{
    auto data = plainTextEdit->currentBlockData();
    logDetailsWidget->setLog(data ? static_cast<Git::Log *>(data->data) : nullptr);
}
