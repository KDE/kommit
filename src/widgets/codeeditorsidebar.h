/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QWidget>

class CodeEditor;
class CodeEditorSidebar : public QWidget
{
    Q_OBJECT
public:
    explicit CodeEditorSidebar(CodeEditor *editor);
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    CodeEditor *m_codeEditor;
};

