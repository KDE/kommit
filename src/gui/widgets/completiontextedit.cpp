/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "completiontextedit.h"
#include <QAbstractItemView>
#include <QCompleter>
#include <QKeyEvent>
#include <QScrollBar>
#include <QStringListModel>

CompletionTextEdit::CompletionTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , mCompletionModel(new QStringListModel(this))
    , mCompleter(new QCompleter(this))
{
    mCompleter->setWidget(this);
    mCompleter->setModel(mCompletionModel);
    mCompleter->setCompletionMode(QCompleter::PopupCompletion);
    mCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(mCompleter, QOverload<const QString &>::of(&QCompleter::activated), this, &CompletionTextEdit::insertCompletion);
}

void CompletionTextEdit::setCompleter(QCompleter *completer)
{
    Q_UNUSED(completer)

    if (mCompleter)
        mCompleter->disconnect(this);
}

QCompleter *CompletionTextEdit::completer() const
{
    return mCompleter;
}

void CompletionTextEdit::addWord(const QString &word)
{
    mWords.append(word);
}

void CompletionTextEdit::addWords(const QStringList &words)
{
    mWords.append(words);
}

void CompletionTextEdit::insertCompletion(const QString &completion)
{
    if (mCompleter->widget() != this)
        return;
    QTextCursor tc = textCursor();
    const int extra = completion.length() - mCompleter->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CompletionTextEdit::textUnderCursor() const
{
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

const QStringList &CompletionTextEdit::words() const
{
    return mWords;
}

void CompletionTextEdit::setWords(const QStringList &newWords)
{
    mWords = newWords;
}

void CompletionTextEdit::begin()
{
    mCompletionModel->setStringList(mWords);
}

void CompletionTextEdit::focusInEvent(QFocusEvent *e)
{
    if (mCompleter)
        mCompleter->setWidget(this);
    QTextEdit::focusInEvent(e);
}

void CompletionTextEdit::keyPressEvent(QKeyEvent *e)
{
    if (mCompleter && mCompleter->popup()->isVisible()) {
        // The following keys are forwarded by the completer to the widget
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return; // let the completer do default behavior
        default:
            break;
        }
    }

    const bool isShortcut = (e->modifiers().testFlag(Qt::ControlModifier) && e->key() == Qt::Key_E); // CTRL+E
    if (!mCompleter || !isShortcut) // do not process the shortcut when we have a completer
        QTextEdit::keyPressEvent(e);

    const bool ctrlOrShift = e->modifiers().testFlag(Qt::ControlModifier) || e->modifiers().testFlag(Qt::ShiftModifier);
    if (!mCompleter || (ctrlOrShift && e->text().isEmpty()))
        return;

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    const bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();

    if (!isShortcut && (hasModifier || e->text().isEmpty() || completionPrefix.length() < 3 || eow.contains(e->text().right(1)))) {
        mCompleter->popup()->hide();
        return;
    }

    if (completionPrefix != mCompleter->completionPrefix()) {
        mCompleter->setCompletionPrefix(completionPrefix);
        mCompleter->popup()->setCurrentIndex(mCompleter->completionModel()->index(0, 0));
    }
    QRect cr = cursorRect();
    cr.setWidth(mCompleter->popup()->sizeHintForColumn(0) + mCompleter->popup()->verticalScrollBar()->sizeHint().width());
    mCompleter->complete(cr); // popup it up!
}
