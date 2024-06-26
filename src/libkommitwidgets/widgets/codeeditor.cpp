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

class BlockUserData : public QTextBlockUserData
{
public:
    CodeEditor::BlockData *data;
    QString extraText;
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
    , mHighlighter(new KSyntaxHighlighting::SyntaxHighlighter(document()))
    , mSideBar(new CodeEditorSidebar(this))
    , mTitleBar(new QLabel(this))
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    setWordWrapMode(QTextOption::NoWrap);

    setTheme((palette().color(QPalette::Base).lightness() < 128) ? mRepository.defaultTheme(KSyntaxHighlighting::Repository::DarkTheme)
                                                                 : mRepository.defaultTheme(KSyntaxHighlighting::Repository::LightTheme));

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

    mFormats.insert(Added, addedFormat);
    mFormats.insert(Removed, removedFormat);
    mFormats.insert(Unchanged, normalFormat);
    mFormats.insert(Edited, changedFormat);
    mFormats.insert(HighLight, highlightFormat);
    mFormats.insert(Empty, emptyFormat);
    mFormats.insert(Odd, oddFormat);
    mFormats.insert(Even, evenFormat);

    setLineWrapMode(QPlainTextEdit::NoWrap);

    mTitleBar->setAlignment(Qt::AlignCenter);
    mTitlebarDefaultHeight = mTitleBar->fontMetrics().height() + 4;
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
    auto pal = qApp->palette();
    if (theme.isValid()) {
        pal.setColor(QPalette::Base, theme.editorColor(KSyntaxHighlighting::Theme::BackgroundColor));
        pal.setColor(QPalette::Highlight, theme.editorColor(KSyntaxHighlighting::Theme::TextSelection));
    }
    setPalette(pal);

    mHighlighter->setTheme(theme);
    mHighlighter->rehighlight();
    highlightCurrentLine();

    mTitleBar->setPalette(pal);
    mTitleBar->setStyleSheet(QStringLiteral("border: 1px solid %1; border-width: 0 0 1 0;")
                                 .arg(QColor(theme.editorColor(KSyntaxHighlighting::Theme::IconBorder)).darker(200).name()));
}

int CodeEditor::sidebarWidth() const
{
    const auto longestText = std::max_element(mBlocksData.begin(), mBlocksData.end(), [](BlockData *d1, BlockData *d2) {
        return d1->extraText.size() < d2->extraText.size();
    });
    int count = int(std::log10(blockCount() + 1));
    if (longestText != mBlocksData.end())
        count += longestText.value()->extraText.size() + 3;
    return 4 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * count + fontMetrics().lineSpacing();
}

int CodeEditor::titlebarHeight() const
{
    return mShowTitlebar ? mTitlebarDefaultHeight : 0;
}

void CodeEditor::sidebarPaintEvent(QPaintEvent *event)
{
    QPainter painter(mSideBar);
    painter.fillRect(event->rect(), mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::IconBorder));

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    const int currentBlockNumber = textCursor().blockNumber();

    const auto foldingMarkerSize = fontMetrics().lineSpacing();
    int lineNumber{0};

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QBrush bg;
            if (blockNumber >= mCurrentSegment.first && blockNumber <= mCurrentSegment.second)
                bg = Qt::yellow;
            else
                bg = document()->findBlockByNumber(blockNumber).blockFormat().background();
            painter.fillRect(QRect{0, top, mSideBar->width() - 1, fontMetrics().height()}, bg);

            painter.setPen(mHighlighter->theme().editorColor((blockNumber == currentBlockNumber) ? KSyntaxHighlighting::Theme::CurrentLineNumber
                                                                                                 : KSyntaxHighlighting::Theme::LineNumbers));
            auto data = mBlocksData.value(block, nullptr);
            lineNumber = data ? data->lineNumber : -1;

            if (data && !data->extraText.isEmpty()) {
                painter.drawText(0, top, mSideBar->width() - 2 - foldingMarkerSize, fontMetrics().height(), Qt::AlignLeft, data->extraText);
            }
            if (lineNumber != -1) {
                const auto number = QString::number(lineNumber);
                painter.drawText(0, top, mSideBar->width() - 2 - (mShowFoldMarks ? foldingMarkerSize : 9), fontMetrics().height(), Qt::AlignRight, number);
            }
        }

        // folding marker
        if (mShowFoldMarks && block.isVisible() && isFoldable(block)) {
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
            painter.setBrush(QColor(mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::CodeFolding)));
            painter.translate(mSideBar->width() - foldingMarkerSize, top);
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
    return mBlocksData.value(textCursor().block(), nullptr);
}

void CodeEditor::updateViewPortGeometry()
{
    auto th = this->titlebarHeight();
    setViewportMargins(this->sidebarWidth(), th, 0, 0);
    const auto r = contentsRect();
    mSideBar->setGeometry(QRect(r.left(), r.top() + th, sidebarWidth(), r.height()));

    if (th)
        mTitleBar->setGeometry(QRect(r.left(), r.top(), r.width(), th));
    mTitleBar->setVisible(th);
}

void CodeEditor::updateSidebarArea(const QRect &rect, int dy)
{
    if (dy)
        mSideBar->scroll(0, dy);
    else
        mSideBar->update(0, rect.y(), mSideBar->width(), rect.height());
}

void CodeEditor::highlightCurrentLine()
{
    QTextEdit::ExtraSelection selection;
    auto color = QColor(mHighlighter->theme().editorColor(KSyntaxHighlighting::Theme::CurrentLine));
    color.setAlpha(220);
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
    return mHighlighter->startsFoldingRegion(block);
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
    if (!mShowFoldMarks)
        return;

    // we also want to fold the last line of the region, therefore the ".next()"
    const auto endBlock = mHighlighter->findFoldingRegionEnd(startBlock).next();

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

bool CodeEditor::showFoldMarks() const
{
    return mShowFoldMarks;
}

void CodeEditor::setShowFoldMarks(bool newShowFoldMarks)
{
    mShowFoldMarks = newShowFoldMarks;
}

bool CodeEditor::showTitleBar() const
{
    return mShowTitlebar;
}

void CodeEditor::setShowTitleBar(bool newShowTitleBar)
{
    mShowTitlebar = newShowTitleBar;
    mTitleBar->setVisible(newShowTitleBar);
    updateViewPortGeometry();
}

QString CodeEditor::title() const
{
    return mTitleBar->text();
}

void CodeEditor::setTitle(const QString &title)
{
    mTitleBar->setText(title);
    updateViewPortGeometry();
}

void CodeEditor::paintEvent(QPaintEvent *e)
{
    QPlainTextEdit::paintEvent(e);

    //    QPainter p(viewport());
    //    for (auto i = _lines.begin(); i != _lines.end(); ++i) {
    ////        auto b = document()->findBlockByLineNumber(i.key());
    //        auto rc = blockBoundingGeometry(i.key());
    //        rc.moveTop(rc.top() - 2);
    //        rc.setBottom(rc.top() + 2);
    //        p.fillRect(rc, _formats.value(i.value()).background());
    //    }
    viewport()->update();
}

int CodeEditor::lineNumberOfBlock(const QTextBlock &block) const
{
    auto b = mBlocksData.value(block, nullptr);
    return b ? b->lineNumber : -1;
}

void CodeEditor::setHighlighting(const QString &fileName)
{
    const auto def = mRepository.definitionForFileName(fileName);
    mHighlighter->setDefinition(def);
    mTitleBar->setText(fileName);
}

void CodeEditor::append(const QString &code, CodeEditor::BlockType type, Diff::Segment *segment)
{
    auto t = textCursor();

    if (mSegments.size()) {
        t.insertBlock();
    }

    if (!code.isEmpty())
        t.insertText(code);

    mSegments.insert(t.block().blockNumber(), segment);
    t.setBlockFormat(mFormats.value(type));

    if (type != Empty)
        mBlocksData.insert(t.block(), new BlockData{++mLastLineNumber, segment, type});
}

int CodeEditor::append(const QString &code, const QColor &backgroundColor)
{
    auto t = textCursor();

    if (mSegments.size())
        t.insertBlock();

    QTextCursor c(t.block());
    c.insertText(code);
    QTextBlockFormat fmt;
    fmt.setBackground(backgroundColor);
    t.setBlockFormat(fmt);
    mSegments.insert(t.block().blockNumber(), nullptr);

    mBlocksData.insert(t.block(), new BlockData{++mLastLineNumber, nullptr, mLastLineNumber ? BlockType::Odd : BlockType::Even});
    mLastOddEven = !mLastOddEven;

    return t.block().blockNumber();
}

void CodeEditor::append(const QStringList &code, CodeEditor::BlockType type, Diff::Segment *segment, int size)
{
    for (auto &e : code)
        append(e, type, segment);
    for (int var = 0; var < size - code.size(); ++var)
        append(QString(), Empty, segment);
}

int CodeEditor::append(const QString &code, CodeEditor::BlockType type, BlockData *data)
{
    auto t = textCursor();

    // if (mSegments.size())
    // t.insertBlock();

    auto lines = code.split('\n');
    auto d = new BlockUserData;
    d->data = data;
    d->extraText = data->extraText;
    for (auto const &line : std::as_const(lines)) {
        // t.insertBlock();
        // t.block().setUserData(d);

        mBlocksData.insert(t.block(), data);
        auto f = mFormats.value(type);
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

int CodeEditor::currentLineNumber() const
{
    return textCursor().block().firstLineNumber();
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
    for (auto i = mSegments.begin(); i != mSegments.end(); i++) {
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
    return mSegments.value(textCursor().block().blockNumber(), nullptr);
}

void CodeEditor::highlightSegment(Diff::Segment *segment)
{
    mCurrentSegment = qMakePair(-1, -1);
    for (auto i = mSegments.begin(); i != mSegments.end(); i++) {
        if (i.value() == segment) {
            if (mCurrentSegment.first == -1)
                mCurrentSegment.first = i.key();
            //            auto block = document()->findBlockByNumber(i.key());

            //            QTextCursor cursor(block);
            ////            cursor.setBlockFormat(_formats.value(HighLight));
            //            setTextCursor(cursor);
            //            return;
        } else if (mCurrentSegment.first != -1) {
            mCurrentSegment.second = i.key() - 1;
            break;
        }
    }
    //    _currentSegment = segment;
    mSideBar->update();
    qCDebug(KOMMIT_WIDGETS_LOG()) << mCurrentSegment;
    return;
    qCDebug(KOMMIT_WIDGETS_LOG()) << "Segment not found";
}

void CodeEditor::clearAll()
{
    mSegments.clear();
    clear();
    mLastLineNumber = 0;
    auto tmp = mBlocksData.values();
    qDeleteAll(tmp);
    mBlocksData.clear();
}

CodeEditor::BlockData::BlockData(int lineNumber, Diff::Segment *segment, CodeEditor::BlockType type)
    : lineNumber{lineNumber}
    , segment{segment}
    , type{type}
{
}

#include "moc_codeeditor.cpp"
