/*
SPDX-FileCopyrightText: 2020-2022 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blamecodeview.h"

#include "blamedata.h"
#include "codeeditorsidebar.h"
#include "gitlog.h"

#include <KLocalizedString>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>

#include <QPainter>

BlameCodeView::BlameCodeView(QWidget *parent)
    : CodeEditor(parent)
{
    setReadOnly(true);
}

const Git::BlameData &BlameCodeView::blameData() const
{
    return mBlameData;
}

void BlameCodeView::setBlameData(const Git::BlameData &newBlameData)
{
    BlockType type{Odd};
    const QVector<QColor> colors{QColor(200, 150, 150, 100), QColor(150, 200, 150, 100)};
    int currentColor{0};
    mBlameData = newBlameData;
    QString lastCommit;
    for (const auto &blame : newBlameData) {
        const QString commitHash = blame.log ? blame.log->commitHash() : QString();

        if (lastCommit != commitHash) {
            currentColor = (currentColor + 1) % colors.size();
            type = type == Odd ? Even : Odd;
        }

        auto data = new BlockData{-1, nullptr, type};
        data->extraText = blame.log ? blame.log->committerName() : i18n("Uncommited");
        data->data = blame.log ? blame.log : nullptr;

        append(blame.code, type, data);
        lastCommit = commitHash;
    }
}

Git::Log *BlameCodeView::currentLog() const
{
    auto tmp = currentBlockData();
    if (tmp)
        return static_cast<Git::Log *>(tmp->data);
    return nullptr;
}
