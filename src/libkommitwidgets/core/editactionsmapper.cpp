/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "editactionsmapper.h"

#include <KActionCollection>
#include <QEvent>
#include <QPlainTextEdit>

#include <KStandardAction>

EditActionsMapper::EditActionsMapper(QObject *parent)
    : QObject(parent)
{
}

void EditActionsMapper::addTextEdit(QPlainTextEdit *control)
{
    mTextEdits.append(control);

    control->installEventFilter(this);

    control->setContextMenuPolicy(Qt::DefaultContextMenu);
    connect(control, &QPlainTextEdit::copyAvailable, this, &EditActionsMapper::controlCopyAvailable);
    connect(control, &QPlainTextEdit::selectionChanged, this, &EditActionsMapper::controlSelectionChanged);
    connect(control, &QPlainTextEdit::undoAvailable, this, &EditActionsMapper::controlUndoAvailable);
    connect(control, &QPlainTextEdit::redoAvailable, this, &EditActionsMapper::controlRedoAvailable);
}

void EditActionsMapper::init(KActionCollection *actionCollection)
{
    mActionCut = KStandardAction::cut(this, &EditActionsMapper::actionCutTriggered, actionCollection);
    mActionCopy = KStandardAction::copy(this, &EditActionsMapper::actionCopyTriggered, actionCollection);
    mActionPaste = KStandardAction::paste(this, &EditActionsMapper::actionPasteTriggered, actionCollection);
    mSelectAll = KStandardAction::selectAll(this, &EditActionsMapper::actionSelectAllTriggered, actionCollection);
    mActionUndo = KStandardAction::undo(this, &EditActionsMapper::actionUndoTriggered, actionCollection);
    mActionRedo = KStandardAction::redo(this, &EditActionsMapper::actionRedoTriggered, actionCollection);
}

void EditActionsMapper::controlUndoAvailable(bool b)
{
    auto control = qobject_cast<QPlainTextEdit *>(sender());
    if (control != mActiveControl)
        return;

    mActionUndo->setEnabled(b);
}

void EditActionsMapper::controlRedoAvailable(bool b)
{
    auto control = qobject_cast<QPlainTextEdit *>(sender());
    if (control != mActiveControl)
        return;

    mActionRedo->setEnabled(b);
}

void EditActionsMapper::controlCopyAvailable(bool b)
{
    auto control = qobject_cast<QPlainTextEdit *>(sender());
    if (control != mActiveControl)
        return;

    mActionCopy->setEnabled(b);
}

void EditActionsMapper::controlSelectionChanged()
{
}

void EditActionsMapper::actionUndoTriggered()
{
    if (mActiveControl)
        mActiveControl->undo();
}

void EditActionsMapper::actionRedoTriggered()
{
    if (mActiveControl)
        mActiveControl->redo();
}

void EditActionsMapper::actionCopyTriggered()
{
    if (mActiveControl)
        mActiveControl->copy();
}

void EditActionsMapper::actionCutTriggered()
{
    if (mActiveControl)
        mActiveControl->cut();
}

void EditActionsMapper::actionPasteTriggered()
{
    if (mActiveControl)
        mActiveControl->paste();
}

void EditActionsMapper::actionSelectAllTriggered()
{
    if (mActiveControl)
        mActiveControl->selectAll();
}

void EditActionsMapper::actionDeleteTriggered()
{
}

bool EditActionsMapper::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::FocusIn && event->type() != QEvent::FocusOut)
        return QObject::eventFilter(watched, event);
    auto textEdit = qobject_cast<QPlainTextEdit *>(watched);
    if (!textEdit || textEdit == mActiveControl)
        return QObject::eventFilter(watched, event);
    //    auto e = static_cast<QFocusEvent*>(event);

    if (event->type() != QEvent::FocusIn)
        mActiveControl = textEdit;
    else
        mActiveControl = nullptr;

    return QObject::eventFilter(watched, event);
}

// #include "moc_editactionsmapper.cpp"
