/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "codeeditorsidebar.h"
#include "codeeditor.h"

#include <QTextBlock>

CodeEditorSidebar::CodeEditorSidebar(CodeEditor *editor)
    : QWidget(editor)
    , m_codeEditor(editor)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
}

QSize CodeEditorSidebar::sizeHint() const
{
    return QSize(m_codeEditor->sidebarWidth(), 0);
}

void CodeEditorSidebar::paintEvent(QPaintEvent *event)
{
    m_codeEditor->sidebarPaintEvent(event);
}

void CodeEditorSidebar::mouseReleaseEvent(QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    auto x = event->position().x();
    auto y = event->position().y();
#else
    auto x = event->x();
    auto y = event->y();
#endif

    if (x >= width() - m_codeEditor->fontMetrics().lineSpacing()) {
        const auto block = m_codeEditor->blockAtPosition(y);
        if (!block.isValid() || !m_codeEditor->isFoldable(block))
            return;
        m_codeEditor->toggleFold(block);
    }
    QWidget::mouseReleaseEvent(event);
}

#include "moc_codeeditorsidebar.cpp"
