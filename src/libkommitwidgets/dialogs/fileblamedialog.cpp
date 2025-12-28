/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "fileblamedialog.h"

#include <Kommit/Blame>
#include <Kommit/Blob>
#include <Kommit/Repository>

#include "models/commitsmodel.h"

#include <KLocalizedString>
#include <QThread>

FileBlameDialog::FileBlameDialog(Git::Repository *git, const QString &file, QWidget *parent)
    : AppDialog(git, parent)
    , mFile(file)
{
    setupUi(this);

    widgetCommitDetails->setRepo(git);

    widgetCommitDetails->setEnableCommitsLinks(false);
    plainTextEdit->setShowTitleBar(false);

    connect(plainTextEdit, &BlameCodeView::blockSelected, this, &FileBlameDialog::slotPlainTextEditBlockSelected);

    loadData();

    widgetCommitDetails->setEnableCommitsLinks(false);
    widgetCommitDetails->setEnableEmailsLinks(false);
    widgetCommitDetails->setEnableFilesLinks(false);
}

void FileBlameDialog::loadData()
{
    plainTextEdit->setHighlighting(mFile);

    mBlameData = mGit->blame(mFile);

    auto hunks = mBlameData.hunks();
    auto type = CodeEditor::BlockType::Odd;
    QList<CodeEditor::BlockData *> blocks;
    for (auto &blame : hunks) {
        auto data = new CodeEditor::BlockData{static_cast<int>(blame.startLine() - 1), static_cast<int>(blame.linesCount()), 0, type};
        if (blame.finalCommit().isNull()) {
            data->extraText = i18n("Uncommitted");
            data->type = CodeEditor::BlockType::Empty;
        } else {
            data->extraText = blame.finalCommit().oid().toString();
            type = type == CodeEditor::BlockType::Odd ? CodeEditor::BlockType::Even : CodeEditor::BlockType::Odd;
        }
        blocks << data;
    }
    plainTextEdit->setContent(mBlameData.content(), blocks, false);
    setWindowTitle(i18nc("@title:window", "Blame file: %1", mFile));
}

void FileBlameDialog::slotPlainTextEditBlockSelected()
{
    auto hunk = mBlameData.hunkByLineNumber(plainTextEdit->currentLineNumber());
    widgetCommitDetails->setCommit(hunk.finalCommit());
}

#include "moc_fileblamedialog.cpp"
