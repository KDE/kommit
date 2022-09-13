/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileblamedialog.h"

#include "git/gitmanager.h"
#include "git/models/logsmodel.h"
#include "qtextobject.h"

#include <klocalizedstring.h>

FileBlameDialog::FileBlameDialog(Git::Manager *git, const QString &fileName, QWidget *parent)
    : AppDialog(parent)
    , mGit(git)
    , mFileName(fileName)
{
    setupUi(this);

    plainTextEdit->setHighlighting(fileName);
    auto content = mGit->fileContent("", mFileName);
    plainTextEdit->setPlainText(content);

    setWindowTitle(i18nc("@title:window", "Blame file: %1", fileName));
    logDetailsWidget->setEnableCommitsLinks(false);
}

FileBlameDialog::FileBlameDialog(const Git::File &file, QWidget *parent)
    : AppDialog(parent)
    , mGit(Git::Manager::instance())
    , mFile(file)
{
    setupUi(this);
    plainTextEdit->setHighlighting(file.fileName());
    //    plainTextEdit->setPlainText(file.content());

    auto b = file.git()->blame(file);
    plainTextEdit->setBlameData(b);

    //    b.initCommits(file.git()->logs());
    setWindowTitle(i18nc("@title:window", "Blame file: %1", file.fileName()));

    logDetailsWidget->setEnableCommitsLinks(false);
}

void FileBlameDialog::on_plainTextEdit_blockSelected()
{
    auto b = plainTextEdit->blameData(plainTextEdit->textCursor().block().blockNumber());
    logDetailsWidget->setLog(mGit->logsModel()->findLogByHash(b.commitHash));
}
