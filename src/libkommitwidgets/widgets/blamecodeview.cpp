/*
SPDX-FileCopyrightText: 2020-2022 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blamecodeview.h"

#include <entities/commit.h>

#include <KLocalizedString>
#include <KSyntaxHighlighting/SyntaxHighlighter>

BlameCodeView::BlameCodeView(QWidget *parent)
    : CodeEditor(parent)
{
    setReadOnly(true);
}

void BlameCodeView::setBlameData(QSharedPointer<Git::BlameData> newBlameData)
{
    BlockType type{Odd};
    const QVector<QColor> colors{QColor(200, 150, 150, 100), QColor(150, 200, 150, 100)};
    int currentColor{0};
    mBlameData = newBlameData;
    QString lastCommit;
    auto hunks = newBlameData->hunks();
    for (const auto &blame : hunks) {
        const QString commitHash = blame->finalCommit ? blame->finalCommit->commitHash() : QString();

        if (lastCommit != commitHash) {
            currentColor = (currentColor + 1) % colors.size();
            type = type == Odd ? Even : Odd;
        }

        auto data = new BlockData{-1, nullptr, type};
        data->extraText = blame->finalCommit ? blame->finalCommit->committer()->name() : i18n("Uncommited");
        data->data = blame->finalCommit.data();

        appendCode(mBlameData->codeLines(blame), type);
        lastCommit = commitHash;
    }
}

#include "moc_blamecodeview.cpp"
