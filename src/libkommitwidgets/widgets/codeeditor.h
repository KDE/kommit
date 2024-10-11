/*
 SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

 SPDX-License-Identifier: GPL-3.0-or-later
 */

#pragma once

#include <KSyntaxHighlighting/Repository>
#include <QMap>
#include <QPlainTextEdit>
#include <QScopedPointer>
#include <QTextBlock>
#include <QTextBlockFormat>

#include <diff.h>

#include "libkommitwidgets_export.h"

class QLabel;

namespace KSyntaxHighlighting
{
class SyntaxHighlighter;
}

class CodeEditorSidebar;

class CodeEditorPrivate;
class LIBKOMMITWIDGETS_EXPORT CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    enum BlockType { Unchanged, Added, Removed, Edited, HighLight, Odd, Even, Empty };
    struct BlockData {
        int lineNumber;
        int lineCount;
        int maxLineCount;

        Diff::Segment *segment;
        BlockType type;
        QString extraText;
        void *data;

        QTextBlock firstBlock;
        QTextBlock endBlock;

        BlockData(int lineNumber, Diff::Segment *segment, CodeEditor::BlockType type);
        BlockData(int lineNumber, int lineCount, int maxLineCount, CodeEditor::BlockType type);
    };

    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override;

    void setHighlighting(const QString &fileName);

    Q_DECL_DEPRECATED
    void append(const QString &code, CodeEditor::BlockType type = Unchanged, Diff::Segment *segment = nullptr);
    Q_DECL_DEPRECATED
    int append(const QString &code, const QColor &backgroundColor);
    void append(const QStringList &code, CodeEditor::BlockType type = Unchanged, Diff::Segment *segment = nullptr, int size = -1);
    Q_DECL_DEPRECATED
    void append(const QList<QStringView> &code, CodeEditor::BlockType type = Unchanged, Diff::Segment *segment = nullptr, int size = -1);
    Q_DECL_DEPRECATED
    int append(const QString &code, CodeEditor::BlockType type, BlockData *data);

    void appendLines(const QStringList &content, BlockData *data, bool fill);
    void setContent(const QStringList &content, QList<BlockData *> dataList, bool fill);

    void appendCode(const QStringList &code, CodeEditor::BlockType type = Unchanged, int fillSize = -1);
    int addFrame(const QStringList &lines, CodeEditor::BlockType type = Removed);
    void setFrameText(int index, const QStringList &lines);

    void highLight(int from, int to);
    QPair<int, int> blockArea(int from, int to);
    QPair<int, int> visibleLines() const;

    [[nodiscard]] int currentLineNumber() const;
    void gotoLineNumber(int lineNumber);
    void gotoSegment(Diff::Segment *segment);

    Diff::Segment *currentSegment() const;
    void highlightSegment(Diff::Segment *segment);

    void clearAll();

    [[nodiscard]] bool showTitleBar() const;
    void setShowTitleBar(bool newShowTitleBar);

    [[nodiscard]] QString title() const;
    void setTitle(const QString &title);

    [[nodiscard]] int titlebarHeight() const;

    [[nodiscard]] bool showFoldMarks() const;
    void setShowFoldMarks(bool newShowFoldMarks);

    [[nodiscard]] BlockData *currentBlockData() const;

    void setBlocksData(QList<BlockData *> list);
Q_SIGNALS:
    void blockSelected();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    // void paintEvent(QPaintEvent *e) override;
    int sidebarWidth() const;
    void sidebarPaintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT int lineNumberOfBlock(const QTextBlock &block) const;

    LIBKOMMITWIDGETS_NO_EXPORT void setTheme(const KSyntaxHighlighting::Theme &theme);

    LIBKOMMITWIDGETS_NO_EXPORT void updateViewPortGeometry();
    LIBKOMMITWIDGETS_NO_EXPORT void updateSidebarArea(const QRect &rect, int dy);
    LIBKOMMITWIDGETS_NO_EXPORT void highlightCurrentLine();

    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT QTextBlock blockAtPosition(int y) const;
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT bool isFoldable(const QTextBlock &block) const;
    [[nodiscard]] LIBKOMMITWIDGETS_NO_EXPORT bool isFolded(const QTextBlock &block) const;
    LIBKOMMITWIDGETS_NO_EXPORT void toggleFold(const QTextBlock &block);

    friend class CodeEditorSidebar;

    QScopedPointer<CodeEditorPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CodeEditor)
};
