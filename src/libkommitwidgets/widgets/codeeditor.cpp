/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "codeeditor.h"
#include "codeeditorsidebar.h"
#include "kommitwidgetsglobaloptions.h"
#include "libkommitwidgets_appdebug.h"

#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/FoldingRegion>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>

#include <QApplication>
#include <QFontDatabase>
#include <QLabel>
#include <QPainter>
#include <QPalette>

#include <QtMath>

class CodeEditorPrivate
{
    CodeEditor *q_ptr;
    Q_DECLARE_PUBLIC(CodeEditor)

public:
    CodeEditorPrivate(CodeEditor *parent);

    int lastPlaceholderIndex{};
    QList<CodeEditor::BlockData *> dataList;
    bool fill{false};
    bool isEmpty{true};

    KSyntaxHighlighting::SyntaxHighlighter *const mHighlighter;
    CodeEditorSidebar *const mSideBar;
    QLabel *mTitleBar;

    QMap<CodeEditor::BlockType, QTextBlockFormat> mFormats;
    KSyntaxHighlighting::Repository mRepository;
    QMap<int, Diff::Segment *> mSegments;
    QMap<QTextBlock, CodeEditor::BlockData *> mBlocksData;
    QList<CodeEditor::BlockData *> mBlocks;

    QPair<int, int> mCurrentSegment{-1, -1};
    QList<QTextBlock> readOnlyBlocks;

    bool mShowTitlebar = true;
    int mTitlebarDefaultHeight;

    int mLastLineNumber{0};
    bool mShowFoldMarks{false};
    bool mLastOddEven{false};

    int highlightStart{-1};
    int highlightEnd{-1};
    struct PlaceholderInfo {
        QTextBlock block;
        qsizetype linesCount;
    };
    QMap<int, PlaceholderInfo> placeholderBlocks;
    QList<QPair<int, int>> emptyBlocks;

    CodeEditor::BlockData *findBlockData(const QTextBlock &block)
    {
        if (!dataList.size())
            return nullptr;
        auto ln = block.firstLineNumber();

        for (auto &data : dataList) {
            auto &max = fill ? data->maxLineCount : data->lineCount;

            if (data->lineNumber <= ln && ln <= data->lineNumber + max)
                return data;

            ln += max;
        }
        return nullptr;

        auto it = fill ? std::find_if(dataList.begin(),
                                      dataList.end(),
                                      [&ln](auto data) {
                                          return data->lineNumber >= ln && data->maxLineCount + data->lineNumber <= ln;
                                      })
                       : std::find_if(dataList.begin(), dataList.end(), [&ln](CodeEditor::BlockData *data) {
                             return data->lineNumber >= ln && data->lineCount + data->lineNumber <= ln;
                         });
        if (it != dataList.end())
            return *it;
        return nullptr;
    }

    int mapBlockNumber(int blockNumber)
    {
        if (!fill)
            return blockNumber + 1;

        auto ln = blockNumber + 1;

        auto n{0};
        for (auto &data : std::as_const(dataList)) {
            if (ln <= n + data->maxLineCount) {
                if (ln > n + data->lineCount)
                    return -1;

                return std::min(data->lineNumber + data->lineCount, ln);
            }
            n += data->maxLineCount;
        }
        return 0;
    }
};
CodeEditorPrivate::CodeEditorPrivate(CodeEditor *parent)
    : q_ptr{parent}
    , mHighlighter{new KSyntaxHighlighting::SyntaxHighlighter{parent->document()}}
    , mSideBar{new CodeEditorSidebar{parent}}
    , mTitleBar{new QLabel{parent}}
{
}

class BlockUserData : public QTextBlockUserData
{
public:
    CodeEditor::BlockData *data;
    QString extraText;
    bool isPlaceholder{false};
    int index;
};

class LIBKOMMITWIDGETS_EXPORT SegmentData : public QTextBlockUserData
{
public:
    SegmentData(Diff::Segment *segment, int lineNumber, bool empty = false);
    void setSegment(Diff::Segment *newSegment);

    Diff::Segment *segment() const;
    int mLineNumber{1};
    bool mIsEmpty{false};

private:
    Diff::Segment *mSegment{nullptr};
};

SegmentData::SegmentData(Diff::Segment *segment, int lineNumber, bool empty)
    : mLineNumber(lineNumber)
    , mIsEmpty(empty)
    , mSegment(segment)
{
}

Diff::Segment *SegmentData::segment() const
{
    return mSegment;
}

void SegmentData::setSegment(Diff::Segment *newSegment)
{
    mSegment = newSegment;
}

CodeEditor::CodeEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , d_ptr{new CodeEditorPrivate{this}}

{
    Q_D(CodeEditor);

    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setWordWrapMode(QTextOption::NoWrap);

    setTheme((palette().color(QPalette::Base).lightness() < 128) ? d->mRepository.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                                                                 : d->mRepository.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));

    connect(this, &QPlainTextEdit::blockCountChanged, this, &CodeEditor::updateViewPortGeometry);
    connect(this, &QPlainTextEdit::updateRequest, this, &CodeEditor::updateSidebarArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &CodeEditor::highlightCurrentLine);

    highlightCurrentLine();

    QTextBlockFormat normalFormat, addedFormat, removedFormat, changedFormat, highlightFormat, emptyFormat, oddFormat, evenFormat;

    addedFormat.setBackground(KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Added));
    removedFormat.setBackground(KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Removed));
    changedFormat.setBackground(KommitWidgetsGlobalOptions::instance()->statucColor(Git::ChangeStatus::Modified));
    highlightFormat.setBackground(Qt::yellow);
    emptyFormat.setBackground(Qt::gray);
    oddFormat.setBackground(QColor(200, 150, 150, 100));
    evenFormat.setBackground(QColor(150, 200, 150, 100));
    //    normalFormat.setBackground(Qt::lightGray);

    d->mFormats.insert(Added, addedFormat);
    d->mFormats.insert(Removed, removedFormat);
    d->mFormats.insert(Unchanged, normalFormat);
    d->mFormats.insert(Edited, changedFormat);
    d->mFormats.insert(HighLight, highlightFormat);
    d->mFormats.insert(Empty, emptyFormat);
    d->mFormats.insert(Odd, oddFormat);
    d->mFormats.insert(Even, evenFormat);

    setLineWrapMode(QPlainTextEdit::NoWrap);

    d->mTitleBar->setAlignment(Qt::AlignCenter);
    d->mTitlebarDefaultHeight = d->mTitleBar->fontMetrics().height() + 4;
    updateViewPortGeometry();
}

CodeEditor::~CodeEditor()
{
    clearAll();
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    updateViewPortGeometry();
}

void CodeEditor::mousePressEvent(QMouseEvent *event)
{
    QPlainTextEdit::mousePressEvent(event);
}

void CodeEditor::setTheme(const KSyntaxHighlighting::Theme &theme)
{
    Q_D(CodeEditor);

    auto pal = qApp->palette();
    if (theme.isValid()) {
        pal.setColor(QPalette::Base, theme.editorColor(KSyntaxHighlighting::Theme::BackgroundColor));
        pal.setColor(QPalette::Highlight, theme.editorColor(KSyntaxHighlighting::Theme::TextSelection));
    }
    setPalette(pal);

    d->mHighlighter->setTheme(theme);
    d->mHighlighter->rehighlight();
    highlightCurrentLine();

    d->mTitleBar->setPalette(pal);
    d->mTitleBar->setStyleSheet(QStringLiteral("border: 1px solid %1; border-width: 0 0 1 0;")
                                    .arg(QColor(theme.editorColor(KSyntaxHighlighting::Theme::IconBorder)).darker(200).name()));
}

int CodeEditor::sidebarWidth() const
{
    Q_D(const CodeEditor);
    const auto longestText = std::max_element(d->dataList.begin(), d->dataList.end(), [](BlockData *d1, BlockData *d2) {
        return d1->extraText.size() < d2->extraText.size();
    });
    int count = int(std::log10(blockCount() + 1));
    if (longestText != d->dataList.end())
        count += (*longestText)->extraText.size() + 3;
    return 4 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * count + fontMetrics().lineSpacing();
}

int CodeEditor::titlebarHeight() const
{
    Q_D(const CodeEditor);
    return d->mShowTitlebar ? d->mTitlebarDefaultHeight : 0;
}

void CodeEditor::sidebarPaintEvent(QPaintEvent *event)
{
    Q_D(CodeEditor);

    QPainter painter(d->mSideBar);
    painter.fillRect(event->rect(), d->mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::IconBorder));

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    const int currentBlockNumber = textCursor().blockNumber();

    const auto foldingMarkerSize = fontMetrics().lineSpacing();
    auto lines = visibleLines();

    // auto data = d->findBlockData(block);

    // if (!data)
    //     return;

    int lineNumber = block.firstLineNumber();

    // auto it = std::find_if(d->lineNumbersMap.begin(), d->lineNumbersMap.end(), [&lineNumber](const QPair<int, int> &p) {
    //     return p.first > lineNumber;
    // });

    QList<CodeEditor::BlockData *>::iterator it;

    if (blockNumber)
        it = std::find_if(d->dataList.begin(), d->dataList.end(), [&blockNumber](BlockData *data) {
            return data->lineNumber >= blockNumber;
        });
    else
        it = d->dataList.begin();

    bool extraDataPrinted{false};
    while (block.isValid() && lineNumber <= lines.first + lines.second) {
        // if (block.isVisible() && bottom >= event->rect().top()) {
        QBrush bg;
        auto n = d->mapBlockNumber(block.blockNumber());

        // if (d->fill && lineNumber > data->lineNumber + data->lineCount) {

        // }
        // if (lineNumber > data->lineCount + (d->fill ? data->maxLineCount : data->lineCount))
        //     data = d->findBlockData(block);

        // if (!data)
        //     return;
        // if (!Q_UNLIKELY(lineNumber)) {
        //     auto data = d->findBlockData(block);
        //     if (data)
        //         lineNumber = data->lineNumber;
        // }

        // background
        if (block.blockNumber() >= d->highlightStart && block.blockNumber() <= d->highlightEnd)
            bg = Qt::yellow;
        else
            bg = document()->findBlockByNumber(blockNumber).blockFormat().background();
        painter.fillRect(QRect{0, top, d->mSideBar->width(), 1 + fontMetrics().height()}, bg);

        if (n == -1) { // lineNumber < data->lineNumber + data->maxLineCount) {
            QBrush bg2;
            bg2.setStyle(Qt::Dense7Pattern);
            bg2.setColor(Qt::darkGray);
            painter.fillRect(QRect{0, top, d->mSideBar->width(), 1 + fontMetrics().height()}, bg2);
        }

        // linenumber
        painter.setPen(d->mHighlighter->theme().editorColor((blockNumber == currentBlockNumber) ? KSyntaxHighlighting::Theme::CurrentLineNumber
                                                                                                : KSyntaxHighlighting::Theme::LineNumbers));
        auto data = d->mBlocksData.value(block, nullptr);
        // lineNumber = data2 ? data2->lineNumber : -1;
        // extra data
        if (data && !data->extraText.isEmpty()) {
            painter.drawText(0, top, d->mSideBar->width() - 2 - foldingMarkerSize, fontMetrics().height(), Qt::AlignLeft, data->extraText);
        }
        if (d->dataList.size() && it != d->dataList.end()) {
            if (lineNumber >= (*it)->lineNumber + (d->fill ? (*it)->maxLineCount : (*it)->lineCount)) {
                ++it;
                extraDataPrinted = false;
            }
            if (!extraDataPrinted && it != d->dataList.end()) {
                painter.drawText(0, top, d->mSideBar->width() - 2 - foldingMarkerSize, fontMetrics().height(), Qt::AlignLeft, (*it)->extraText);
                extraDataPrinted = true;
            }
        }
        if (true) { // lineNumber<=data->lineNumber + data->lineCount) {

            if (n != -1) {
                const auto number = QString::number(n);

                painter
                    .drawText(0, top, d->mSideBar->width() - 2 - (d->mShowFoldMarks ? foldingMarkerSize : 9), fontMetrics().height(), Qt::AlignRight, number);
            }
        }

        painter.setPen(d->mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::LineNumbers));
        painter.drawLine(d->mSideBar->width() - 1, 0, d->mSideBar->width() - 1, d->mSideBar->height() - 1);
        lineNumber++;
        // }

        // folding marker
        if (d->mShowFoldMarks && block.isVisible() && isFoldable(block)) {
            QPolygonF polygon;
            if (isFolded(block)) {
                polygon << QPointF(foldingMarkerSize * 0.4, foldingMarkerSize * 0.25);
                polygon << QPointF(foldingMarkerSize * 0.4, foldingMarkerSize * 0.75);
                polygon << QPointF(foldingMarkerSize * 0.8, foldingMarkerSize * 0.5);
            } else {
                polygon << QPointF(foldingMarkerSize * 0.25, foldingMarkerSize * 0.4);
                polygon << QPointF(foldingMarkerSize * 0.75, foldingMarkerSize * 0.4);
                polygon << QPointF(foldingMarkerSize * 0.5, foldingMarkerSize * 0.8);
            }
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(d->mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::CodeFolding)));
            painter.translate(d->mSideBar->width() - foldingMarkerSize, top);
            painter.drawPolygon(polygon);
            painter.restore();
        }

        block = block.next();
        if (!block.isValid())
            break;
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}

CodeEditor::BlockData *CodeEditor::currentBlockData() const
{
    // Q_D(const CodeEditor);
    Q_D(const CodeEditor);
    auto ln = textCursor().block().firstLineNumber() + 1;
    if (!d->fill) {
        auto it = std::find_if(d->dataList.begin(), d->dataList.end(), [&ln](BlockData *data) {
            return data->lineNumber >= ln && data->lineCount + data->lineNumber <= ln;
        });
        if (it != d->dataList.end())
            return *it;
        return nullptr;
    }
    auto n{0};
    for (auto &data : std::as_const(d->dataList)) {
        if (ln <= n + data->maxLineCount) {
            return data;
        }
        n += data->maxLineCount;
    }
    return nullptr;
}

void CodeEditor::setBlocksData(QList<BlockData *> list)
{
    Q_D(CodeEditor);
    d->mBlocks = list;
}

void CodeEditor::updateViewPortGeometry()
{
    Q_D(CodeEditor);

    auto th = this->titlebarHeight();
    setViewportMargins(this->sidebarWidth(), th, 0, 0);
    const auto r = contentsRect();
    d->mSideBar->setGeometry(QRect(r.left(), r.top() + th, sidebarWidth(), r.height()));

    if (th)
        d->mTitleBar->setGeometry(QRect(r.left(), r.top(), r.width(), th));
    d->mTitleBar->setVisible(th);
}

void CodeEditor::updateSidebarArea(const QRect &rect, int dy)
{
    Q_D(CodeEditor);
    if (dy)
        d->mSideBar->scroll(0, dy);
    else
        d->mSideBar->update(0, rect.y(), d->mSideBar->width(), rect.height());
}

void CodeEditor::highlightCurrentLine()
{
    Q_D(CodeEditor);
    QTextEdit::ExtraSelection selection;
    auto color = QColor(d->mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::CurrentLine));
    color.setAlpha(160);
    selection.format.setBackground(color);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

QTextBlock CodeEditor::blockAtPosition(int y) const
{
    auto block = firstVisibleBlock();
    if (!block.isValid())
        return {};

    auto top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    do {
        if (top <= y && y <= bottom)
            return block;
        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
    } while (block.isValid());
    return {};
}

bool CodeEditor::isFoldable(const QTextBlock &block) const
{
    Q_D(const CodeEditor);
    return d->mHighlighter->startsFoldingRegion(block);
}

bool CodeEditor::isFolded(const QTextBlock &block) const
{
    if (!block.isValid())
        return false;
    const auto nextBlock = block.next();
    if (!nextBlock.isValid())
        return false;
    return !nextBlock.isVisible();
}

void CodeEditor::toggleFold(const QTextBlock &startBlock)
{
    Q_D(CodeEditor);

    if (!d->mShowFoldMarks)
        return;

    // we also want to fold the last line of the region, therefore the ".next()"
    const auto endBlock = d->mHighlighter->findFoldingRegionEnd(startBlock).next();

    if (isFolded(startBlock)) {
        // unfold
        auto block = startBlock.next();
        while (block.isValid() && !block.isVisible()) {
            block.setVisible(true);
            block.setLineCount(block.layout()->lineCount());
            block = block.next();
        }

    } else {
        // fold
        auto block = startBlock.next();
        while (block.isValid() && block != endBlock) {
            block.setVisible(false);
            block.setLineCount(0);
            block = block.next();
        }
    }

    // redraw document
    document()->markContentsDirty(startBlock.position(), endBlock.position() - startBlock.position() + 1);

    // update scrollbars
    Q_EMIT document()->documentLayout()->documentSizeChanged(document()->documentLayout()->documentSize());
}

void CodeEditor::keyPressEvent(QKeyEvent *event)
{
    Q_D(CodeEditor);
    if (d->readOnlyBlocks.contains(textCursor().block())) {
        event->ignore();
        return;
    }
    QPlainTextEdit::keyPressEvent(event);
}

bool CodeEditor::showFoldMarks() const
{
    Q_D(const CodeEditor);
    return d->mShowFoldMarks;
}

void CodeEditor::setShowFoldMarks(bool newShowFoldMarks)
{
    Q_D(CodeEditor);
    d->mShowFoldMarks = newShowFoldMarks;
}

bool CodeEditor::showTitleBar() const
{
    Q_D(const CodeEditor);
    return d->mShowTitlebar;
}

void CodeEditor::setShowTitleBar(bool newShowTitleBar)
{
    Q_D(CodeEditor);
    d->mShowTitlebar = newShowTitleBar;
    d->mTitleBar->setVisible(newShowTitleBar);
    updateViewPortGeometry();
}

QString CodeEditor::title() const
{
    Q_D(const CodeEditor);
    return d->mTitleBar->text();
}

void CodeEditor::setTitle(const QString &title)
{
    Q_D(CodeEditor);
    d->mTitleBar->setText(title);
    updateViewPortGeometry();
}

// void CodeEditor::paintEvent(QPaintEvent *e)
// {
//     Q_D(CodeEditor);

//     QPlainTextEdit::paintEvent(e);

//     QPainter p(viewport());
//     // p.setOpacity(.3);
//     // p.fillRect(40, 40, 300, 400, Qt::blue);
//     //    for (auto i = _lines.begin(); i != _lines.end(); ++i) {
//     ////        auto b = document()->findBlockByLineNumber(i.key());
//     //        auto rc = blockBoundingGeometry(i.key());
//     //        rc.moveTop(rc.top() - 2);
//     //        rc.setBottom(rc.top() + 2);
//     //        p.fillRect(rc, _formats.value(i.value()).background());
//     //    }
//     auto firstLine = firstVisibleBlock().blockNumber();
//     auto it = std::find_if(d->mBlocks.begin(), d->mBlocks.end(), [&firstLine](BlockData *data) {
//         return data->lineNumber >= firstLine;
//     });
//     auto h = fontMetrics().height();

//     p.setOpacity(0.6);
//     if (it != d->mBlocks.end()) {
//         auto data = *it;

//         auto rc = QRect{0, h * (data->lineNumber - firstLine), width(), h * (data->lineNumber + data->lineCount - firstLine)};
//         p.fillRect(rc, Qt::blue);
//     }
//     viewport()->update();
// }

int CodeEditor::lineNumberOfBlock(const QTextBlock &block) const
{
    Q_D(const CodeEditor);
    auto b = d->mBlocksData.value(block, nullptr);
    return b ? b->lineNumber : -1;
}

void CodeEditor::setHighlighting(const QString &fileName)
{
    Q_D(CodeEditor);
    const auto def = d->mRepository.definitionForFileName(fileName);
    d->mHighlighter->setDefinition(def);
    d->mTitleBar->setText(fileName);
}

void CodeEditor::append(const QString &code, CodeEditor::BlockType type, Diff::Segment *segment)
{
    Q_D(CodeEditor);

    auto t = textCursor();

    if (!d->isEmpty) {
        t.insertBlock();
    }
    d->isEmpty = false;

    if (!code.isEmpty())
        t.insertText(code);

    d->mSegments.insert(t.block().blockNumber(), segment);
    t.setBlockFormat(d->mFormats.value(type));

    if (type != Empty)
        d->mBlocksData.insert(t.block(), new BlockData{++d->mLastLineNumber, segment, type});
}

int CodeEditor::append(const QString &code, const QColor &backgroundColor)
{
    Q_D(CodeEditor);

    auto t = textCursor();

    if (d->mSegments.size())
        t.insertBlock();

    QTextCursor c(t.block());
    c.insertText(code);
    QTextBlockFormat fmt;
    fmt.setBackground(backgroundColor);
    t.setBlockFormat(fmt);
    d->mSegments.insert(t.block().blockNumber(), nullptr);

    d->mBlocksData.insert(t.block(), new BlockData{++d->mLastLineNumber, nullptr, d->mLastLineNumber ? BlockType::Odd : BlockType::Even});
    d->mLastOddEven = !d->mLastOddEven;

    return t.block().blockNumber();
}

void CodeEditor::append(const QStringList &code, CodeEditor::BlockType type, Diff::Segment *segment, int size)
{
    for (auto &e : code)
        append(e, type, segment);

    for (int var = 0; var < size - code.size(); ++var)
        append(QString(), Empty, segment);
}

void CodeEditor::append(const QList<QStringView> &code, BlockType type, Diff::Segment *segment, int size)
{
    for (auto &e : code)
        append(e.toUtf8(), type, segment); // TODO: check this convert
    // for (int var = 0; var < size - code.size(); ++var)
    append(QString{size - code.size(), QChar(0x00A0)}, Empty, segment);
}

int CodeEditor::append(const QString &code, CodeEditor::BlockType type, BlockData *data)
{
    Q_D(CodeEditor);

    auto t = textCursor();

    // if (mSegments.size())
    // t.insertBlock();

    auto lines = code.split('\n');
    auto blockData = new BlockUserData;
    blockData->data = data;
    blockData->extraText = data->extraText;
    for (auto const &line : std::as_const(lines)) {
        // t.insertBlock();
        // t.block().setUserData(d);

        d->mBlocksData.insert(t.block(), data);
        auto f = d->mFormats.value(type);
        t.setBlockFormat(f);
        t.insertText(line + " \n");
    }
    /*
    data->lineCount = code.split('\n').size();
    mLastLineNumber += data->lineCount;

    QTextCursor c(t.block());

    if (code.endsWith('\n'))
        c.insertText(code);
    else
        c.insertText(code + '\n');

    t.setBlockFormat(mFormats.value(type));
    data->lineNumber = mLastLineNumber;

    mSegments.insert(t.block().blockNumber(), nullptr);
    mBlocksData.insert(t.block(), data);
    mBlocks << data;
*/
    return t.block().blockNumber();
}

void CodeEditor::appendLines(const QStringList &content, BlockData *data, bool fill)
{
    Q_D(CodeEditor);
    auto cursor = textCursor();

    if (!Q_UNLIKELY(d->isEmpty))
        cursor.insertBlock();

    d->isEmpty = false;
    cursor.setBlockFormat(d->mFormats.value(data->type));

    QString s;
    if (content.size())
        s = content.join(QLatin1Char('\n'));
    if (fill && data->maxLineCount > data->lineCount)
        s += QString{data->maxLineCount - data->lineCount - (data->lineCount ? 0 : 1), QLatin1Char('\n')};

    if (data)
        data->firstBlock = cursor.block();
    cursor.insertText(s);

    if (data)
        data->firstBlock = cursor.block();
}

void CodeEditor::setContent(const QStringList &content, QList<BlockData *> dataList, bool fill)
{
    Q_D(CodeEditor);

    d->dataList = dataList;
    d->fill = fill;

    bool first{true};

    clear();
    auto t = textCursor();

    for (auto &data : dataList) {
        QString s;
        if (data->lineCount)
            s = content.mid(data->lineNumber, data->lineCount).join('\n');
        if (fill && data->maxLineCount > data->lineCount)
            s += QString{data->maxLineCount - data->lineCount - (data->lineCount ? 0 : 1), QLatin1Char('\n')};

        if (data->lineCount || (fill && data->maxLineCount)) {
            if (Q_UNLIKELY(first)) {
                first = false;
            } else {
                t.insertBlock();
            }
            QTextCursor c(t.block());
            t.setBlockFormat(d->mFormats.value(data->type));
            c.insertText(s);
        }
    }
}

void CodeEditor::appendCode(const QStringList &code, BlockType type, int fillSize)
{
    Q_D(CodeEditor);
    auto cursor = textCursor();
    // if (!d->isEmpty)
    // cursor.insertBlock();
    d->isEmpty = false;

    cursor.setBlockFormat(d->mFormats.value(type));
    cursor.insertText(code.join('\n') + '\n');

    if (fillSize > code.size()) {
        cursor.setBlockFormat(d->mFormats.value(BlockType::Unchanged));
        cursor.insertText(QString{fillSize - code.size(), QLatin1Char('\n')});
    }
}

int CodeEditor::addFrame(const QStringList &lines, BlockType type)
{
    Q_D(CodeEditor);

    auto cursor = textCursor();

    // if (!d->isEmpty)
    //     cursor.insertBlock();
    // d->isEmpty = false;

    auto index = ++d->lastPlaceholderIndex;
    QTextBlock block = cursor.block();

    cursor.setBlockFormat(d->mFormats.value(type));

    cursor.insertText(lines.join('\n') + '\n');

    d->placeholderBlocks.insert(index, CodeEditorPrivate::PlaceholderInfo{block, lines.size()});

    do {
        d->readOnlyBlocks << block;
        block = block.next();
    } while (block.isValid() && block != cursor.block());
    return index;
}

void CodeEditor::setFrameText(int index, const QStringList &lines)
{
    Q_D(CodeEditor);
    QTextCursor cursor(document());

    // Check if the placeholder block exists
    if (!d->placeholderBlocks.contains(index))
        return;

    // Get placeholder info
    auto info = d->placeholderBlocks.value(index);
    auto startBlock = info.block;
    auto linesCount = info.linesCount;
    auto p = info.block.position();
    // Move cursor to the start block position
    cursor.setPosition(startBlock.position());

    cursor.movePosition(QTextCursor::StartOfBlock);
    // Remove `linesCount` blocks starting from the `startBlock`
    for (int i = 0; i < linesCount - 1; ++i) {
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor);
    }
    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    cursor.removeSelectedText();
    // cursor.deletePreviousChar(); // This will remove the block itself

    // cursor.movePosition(QTextCursor::NextBlock);

    // for (auto &code : lines) {
    //     cursor.insertBlock();
    //     cursor.setBlockFormat(d->mFormats.value(Removed));
    //     cursor.insertText(code);
    // }

    // cursor.setBlockFormat(d->mFormats.value(BlockType::Unchanged));
    // cursor.insertBlock();

    cursor.setBlockFormat(d->mFormats.value(Removed));
    cursor.insertText(lines.join('\n'));

    // cursor.insertBlock();
    // cursor.setBlockFormat(d->mFormats.value(Unchanged));

    d->placeholderBlocks.insert(index, CodeEditorPrivate::PlaceholderInfo{document()->findBlock(p), lines.size()});
}

void CodeEditor::highLight(int from, int to)
{
    Q_D(CodeEditor);
    d->highlightStart = from;
    d->highlightEnd = to;
    d->mSideBar->update();
}

QPair<int, int> CodeEditor::blockArea(int from, int to)
{
    const auto firstBlock = document()->findBlockByLineNumber(from);
    const auto secondBlock = document()->findBlockByLineNumber(to);
    //    qCDebug(KOMMIT_LOG) << from << " to " << to << firstBlock.text() << secondBlock.text();

    int top = qRound(blockBoundingGeometry(firstBlock).translated(contentOffset()).top());
    int bottom;

    if (to == -1)
        bottom = top + 1;
    else
        bottom = qRound(blockBoundingGeometry(secondBlock).translated(contentOffset()).bottom());

    return qMakePair(top, bottom);
}

QPair<int, int> CodeEditor::visibleLines() const
{
    const auto block = firstVisibleBlock();
    auto ret = qMakePair(block.blockNumber(), 0);

    //    while (block.isVisible() && block.isValid()) {
    //        ret.second++;// = mSegmentsLineNumbers.value(block);
    //        block = block.next();
    //    }
    ret.second = (height() - titlebarHeight()) / blockBoundingRect(block).height();
    return ret;
}

void CodeEditor::gotoLineNumber(int lineNumber)
{
    const QTextBlock block = document()->findBlockByLineNumber(lineNumber);

    if (block.isValid()) {
        QTextCursor cursor(block);
        setTextCursor(cursor);
    }
}

void CodeEditor::gotoSegment(Diff::Segment *segment)
{
    Q_D(CodeEditor);
    for (auto i = d->mSegments.begin(); i != d->mSegments.end(); i++) {
        if (i.value() == segment) {
            QTextBlock block = document()->findBlockByLineNumber(i.key());

            if (block.isValid()) {
                QTextCursor cursor(block);
                setTextCursor(cursor);
            }
            return;
        }
    }
}

void CodeEditor::mouseReleaseEvent(QMouseEvent *event)
{
    Q_EMIT blockSelected();
    QPlainTextEdit::mouseReleaseEvent(event);
}

Diff::Segment *CodeEditor::currentSegment() const
{
    Q_D(const CodeEditor);
    return d->mSegments.value(textCursor().block().blockNumber(), nullptr);
}

void CodeEditor::highlightSegment(Diff::Segment *segment)
{
    Q_D(CodeEditor);

    d->mCurrentSegment = qMakePair(-1, -1);
    for (auto i = d->mSegments.begin(); i != d->mSegments.end(); i++) {
        if (i.value() == segment) {
            if (d->mCurrentSegment.first == -1)
                d->mCurrentSegment.first = i.key();
            //            auto block = document()->findBlockByNumber(i.key());

            //            QTextCursor cursor(block);
            ////            cursor.setBlockFormat(_formats.value(HighLight));
            //            setTextCursor(cursor);
            //            return;
        } else if (d->mCurrentSegment.first != -1) {
            d->mCurrentSegment.second = i.key() - 1;
            break;
        }
    }
    //    _currentSegment = segment;
    d->mSideBar->update();
    qCDebug(KOMMIT_WIDGETS_LOG) << d->mCurrentSegment;
    return;
    qCDebug(KOMMIT_WIDGETS_LOG) << "Segment not found";
}

int CodeEditor::currentLineNumber() const
{
    Q_D(const CodeEditor);
    auto ln = textCursor().block().firstLineNumber() + 1;
    if (!d->fill)
        return ln;

    auto n{0};
    for (auto &data : std::as_const(d->dataList)) {
        if (ln <= n + data->maxLineCount) {
            return std::min(data->lineNumber + data->lineCount, ln);
        }
        n += data->maxLineCount;
    }
    return 0;
}

void CodeEditor::clearAll()
{
    Q_D(CodeEditor);

    d->mSegments.clear();
    clear();
    d->mLastLineNumber = 0;
    auto tmp = d->mBlocksData.values();
    qDeleteAll(tmp);
    d->mBlocksData.clear();
    d->isEmpty = true;
    d->emptyBlocks.clear();
}

CodeEditor::BlockData::BlockData(int lineNumber, Diff::Segment *segment, CodeEditor::BlockType type)
    : lineNumber{lineNumber}
    , segment{segment}
    , type{type}
{
}

CodeEditor::BlockData::BlockData(int lineNumber, int lineCount, int maxLineCount, BlockType type)
    : lineNumber{lineNumber}
    , lineCount{lineCount}
    , maxLineCount{maxLineCount}
    , type{type}
{
}

#include "moc_codeeditor.cpp"
