/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QTextEdit>

class QCompleter;
class QStringListModel;
class CompletionTextEdit : public QTextEdit
{
    Q_OBJECT

public:
    explicit CompletionTextEdit(QWidget *parent = nullptr);

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    void addWord(const QString &word);
    void addWords(const QStringList &words);

    const QStringList &words() const;
    void setWords(const QStringList &newWords);

    void begin();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void focusInEvent(QFocusEvent *e) override;

private:
    void insertCompletion(const QString &completion);
    Q_REQUIRED_RESULT QString textUnderCursor() const;

    QStringList mWords;
    QStringListModel *const mCompletionModel;
    QCompleter *const mCompleter;
};
