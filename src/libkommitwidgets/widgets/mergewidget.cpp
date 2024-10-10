/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mergewidget.h"
#include "results.h"

#include <QAction>
#include <QScrollBar>

class MergeWidgetPrivate
{
    MergeWidget *q_ptr;
    Q_DECLARE_PUBLIC(MergeWidget)

public:
    explicit MergeWidgetPrivate(MergeWidget *parent);

    bool sameSize{true};

    QAction *keepMineAction;
    QAction *keepTheirAction;
    QAction *keepMineBeforeTheirAction;
    QAction *keepTheirBeforeMineAction;
    QAction *keepMyFileAction;
    QAction *keepTheirFileAction;
    QAction *gotoPrevDiffAction;
    QAction *gotoNextDiffAction;

    QString baseContent;
    QString localContent;
    QString remoteContent;

    QString baseTitle;
    QString localTitle;
    QString remoteTitle;
    QString resultTitle;

    Diff::MergeResult<Diff::Text> lastMergeResult;
    QList<Diff::MergeSegment2 *>::iterator currentSegment;

    QList<CodeEditor::BlockData *> baseBlockDataList;
    QList<CodeEditor::BlockData *> localBlockDataList;
    QList<CodeEditor::BlockData *> remoteBlockDataList;
    QList<CodeEditor::BlockData *> resultBlockDataList;

    bool isModified;

    QMap<Diff::MergeSegment2 *, int> conflictIndexes;
    QList<Diff::MergeSegment2 *> conflictSegments;

    void resolveConflict(Diff::MergeSegment2 *segment);
    void setEditorContents(QList<Diff::MergeSegment2 *> segments);
    void setCurrentSegment(QList<Diff::MergeSegment2 *>::iterator currentSegment);
};
MergeWidgetPrivate::MergeWidgetPrivate(MergeWidget *parent)
    : q_ptr{parent}
    , keepMineAction{new QAction{parent}}
    , keepTheirAction{new QAction{parent}}
    , keepMineBeforeTheirAction{new QAction{parent}}
    , keepTheirBeforeMineAction{new QAction{parent}}
    , keepMyFileAction{new QAction{parent}}
    , keepTheirFileAction{new QAction{parent}}
    , gotoPrevDiffAction{new QAction{parent}}
    , gotoNextDiffAction{new QAction{parent}}
{
    QObject::connect(keepMineAction, &QAction::triggered, parent, &MergeWidget::slotKeepMineActionTriggered);
    QObject::connect(keepTheirAction, &QAction::triggered, parent, &MergeWidget::slotKeepTheirActionTriggered);
    QObject::connect(keepMineBeforeTheirAction, &QAction::triggered, parent, &MergeWidget::slotKeepMineBeforeTheirActionTriggered);
    QObject::connect(keepTheirBeforeMineAction, &QAction::triggered, parent, &MergeWidget::slotKeepTheirBeforeMineActionTriggered);
    QObject::connect(keepMyFileAction, &QAction::triggered, parent, &MergeWidget::slotKeepMyFileActionTriggered);
    QObject::connect(keepTheirFileAction, &QAction::triggered, parent, &MergeWidget::slotKeepTheirFileActionTriggered);
    QObject::connect(gotoPrevDiffAction, &QAction::triggered, parent, &MergeWidget::slotGotoPrevDiffActionTriggered);
    QObject::connect(gotoNextDiffAction, &QAction::triggered, parent, &MergeWidget::slotGotoNextDiffActionTriggered);

    keepMineAction->setEnabled(false);
    keepTheirAction->setEnabled(false);
    keepMineBeforeTheirAction->setEnabled(false);
    keepTheirBeforeMineAction->setEnabled(false);
    keepMyFileAction->setEnabled(false);
    keepTheirFileAction->setEnabled(false);
    gotoPrevDiffAction->setEnabled(false);
    gotoNextDiffAction->setEnabled(false);
}

void MergeWidgetPrivate::resolveConflict(Diff::MergeSegment2 *segment)
{
    Q_Q(MergeWidget);

    if (segment->type != Diff::MergeSegment2::Type::ChangedOnBoth)
        return;

    if (conflictSegments.removeOne(segment)) {
        Q_EMIT q->conflictsChanged(conflictSegments.size());
        gotoPrevDiffAction->setEnabled(conflictSegments.size());
        gotoNextDiffAction->setEnabled(conflictSegments.size());
    }
    Q_EMIT q->isModifiedChanged(true);
}

void MergeWidgetPrivate::setEditorContents(QList<Diff::MergeSegment2 *> segments)
{
    Q_Q(MergeWidget);

    q->codeEditorBase->clearAll();
    q->codeEditorLocal->clearAll();
    q->codeEditorRemote->clearAll();
    q->codeEditorMyBlock->clearAll();
    q->codeEditorTheirBlock->clearAll();

    q->codeEditorBase->setHighlighting(baseTitle);
    q->codeEditorLocal->setHighlighting(localTitle);
    q->codeEditorRemote->setHighlighting(remoteTitle);
    q->codeEditorResult->setHighlighting(resultTitle);

    q->codeEditorBase->setContent(lastMergeResult.base.lines, baseBlockDataList, sameSize);
    q->codeEditorLocal->setContent(lastMergeResult.local.lines, localBlockDataList, sameSize);
    q->codeEditorRemote->setContent(lastMergeResult.remote.lines, remoteBlockDataList, sameSize);
    // q->codeEditorResult->setContent(lastMergeResult.base.lines, resultBlockDataList, sameSize);
}

void MergeWidgetPrivate::setCurrentSegment(QList<Diff::MergeSegment2 *>::iterator currentSegment)
{
    Q_Q(MergeWidget);
    if (currentSegment != lastMergeResult.segments.end()) {
        keepMineAction->setEnabled((*currentSegment)->type != Diff::MergeSegment2::Type::Unchanged);
        keepTheirAction->setEnabled((*currentSegment)->type != Diff::MergeSegment2::Type::Unchanged);
        keepMineBeforeTheirAction->setEnabled((*currentSegment)->type != Diff::MergeSegment2::Type::Unchanged);
        keepTheirBeforeMineAction->setEnabled((*currentSegment)->type != Diff::MergeSegment2::Type::Unchanged);
    }
    this->currentSegment = currentSegment;

    if (currentSegment != lastMergeResult.segments.end()) {
        auto s = *currentSegment;

        int base{0};
        int local{0};
        int remote{0};
        for (auto &segment : lastMergeResult.segments) {
            if (sameSize) {
                auto m = std::max(std::max(segment->baseSize, segment->localSize), std::max(segment->baseSize, segment->remoteSize));

                if (segment == s) {
                    q->codeEditorBase->highLight(base, base + m - 1);
                    q->codeEditorLocal->highLight(local, local + m - 1);
                    q->codeEditorRemote->highLight(remote, remote + m - 1);
                    break;
                }
                base += m;
                local += m;
                remote += m;
            } else {
                if (segment == s) {
                    q->codeEditorBase->highLight(base, base + segment->baseSize - 1);
                    q->codeEditorLocal->highLight(local, local + segment->localSize - 1);
                    q->codeEditorRemote->highLight(remote, remote + segment->remoteSize - 1);
                    break;
                }
                base += segment->baseSize;
                local += segment->localSize;
                remote += segment->remoteSize;
            }
        }
    }
}

MergeWidget::MergeWidget(QWidget *parent)
    : QWidget(parent)
    , d_ptr{new MergeWidgetPrivate{this}}
{
    setupUi(this);
    widgetPreview->setVisible(false);

    connect(codeEditorBase->verticalScrollBar(), &QScrollBar::valueChanged, this, &MergeWidget::slotCodeEditorScroll);
    connect(codeEditorLocal->verticalScrollBar(), &QScrollBar::valueChanged, this, &MergeWidget::slotCodeEditorScroll);
    connect(codeEditorRemote->verticalScrollBar(), &QScrollBar::valueChanged, this, &MergeWidget::slotCodeEditorScroll);
    connect(codeEditorResult->verticalScrollBar(), &QScrollBar::valueChanged, this, &MergeWidget::slotCodeEditorScroll);

    connect(codeEditorBase, &CodeEditor::blockSelected, this, &MergeWidget::slotCodeBlockSelected);
    connect(codeEditorLocal, &CodeEditor::blockSelected, this, &MergeWidget::slotCodeBlockSelected);
    connect(codeEditorRemote, &CodeEditor::blockSelected, this, &MergeWidget::slotCodeBlockSelected);
}

MergeWidget::~MergeWidget()
{
}

bool MergeWidget::ShowPreview() const
{
    return widgetPreview->isVisible();
}

void MergeWidget::setShowPreview(bool newShowPreview)
{
    widgetPreview->setVisible(newShowPreview);
}

void MergeWidget::setBaseFile(const QString &title, const QString &content)
{
    Q_D(MergeWidget);
    d->baseTitle = title;
    d->baseContent = content;
}

void MergeWidget::setLocalFile(const QString &title, const QString &content)
{
    Q_D(MergeWidget);
    d->localTitle = title;
    d->localContent = content;
}

void MergeWidget::setRemoteFile(const QString &title, const QString &content)
{
    Q_D(MergeWidget);
    d->remoteTitle = title;
    d->remoteContent = content;
}

void MergeWidget::setResultFile(const QString &title)
{
    Q_D(MergeWidget);
    d->resultTitle = title;
}

void MergeWidget::compare()
{
    Q_D(MergeWidget);
    d->lastMergeResult = Diff::diff3String(d->baseContent, d->localContent, d->remoteContent);

    if (d->lastMergeResult.segments.size())
        d->currentSegment = d->lastMergeResult.segments.begin();

    QList<CodeEditor::BlockData *> tmpBaseBlockDataList;
    QList<CodeEditor::BlockData *> tmpLocalBlockDataList;
    QList<CodeEditor::BlockData *> tmpRemoteBlockDataList;
    QList<CodeEditor::BlockData *> tmpResultBlockDataList;

    CodeEditor::BlockType baseType;
    CodeEditor::BlockType localType;
    CodeEditor::BlockType remoteType;
    CodeEditor::BlockType resultType;

    for (auto &segment : d->lastMergeResult.segments) {
        switch (segment->type) {
        case Diff::MergeSegment2::Type::Unchanged:
            baseType = CodeEditor::BlockType::Unchanged;
            localType = CodeEditor::BlockType::Unchanged;
            remoteType = CodeEditor::BlockType::Unchanged;
            resultType = CodeEditor::BlockType::Unchanged;
            codeEditorResult->appendCode(d->lastMergeResult.part(segment, Diff::MergeSide::Base));
            // ->append(d->lastMergeResult.part(segment, Diff::MergeSide::Base), CodeEditor::Unchanged);
            break;
        case Diff::MergeSegment2::Type::OnlyOnLocal:
            baseType = CodeEditor::BlockType::Edited;
            localType = CodeEditor::BlockType::Added;
            remoteType = CodeEditor::BlockType::Removed;
            resultType = CodeEditor::BlockType::Unchanged;
            codeEditorResult->appendCode(d->lastMergeResult.part(segment, Diff::MergeSide::Local), CodeEditor::Added);
            break;
        case Diff::MergeSegment2::Type::OnlyOnRemote:
            baseType = CodeEditor::BlockType::Edited;
            localType = CodeEditor::BlockType::Removed;
            remoteType = CodeEditor::BlockType::Added;
            resultType = CodeEditor::BlockType::Unchanged;
            codeEditorResult->appendCode(d->lastMergeResult.part(segment, Diff::MergeSide::Remote), CodeEditor::Added);

            break;
        case Diff::MergeSegment2::Type::ChangedOnBoth:
            baseType = CodeEditor::BlockType::Edited;
            localType = CodeEditor::BlockType::Edited;
            remoteType = CodeEditor::BlockType::Edited;
            resultType = CodeEditor::BlockType::Removed;

            d->conflictSegments.append(segment);
            d->conflictIndexes.insert(segment, codeEditorResult->addFrame(QStringList{QLatin1String()}));
            break;
        }

        auto m = std::max(std::max(segment->baseSize, segment->localSize), std::max(segment->baseSize, segment->remoteSize));
        tmpBaseBlockDataList.append(new CodeEditor::BlockData{segment->baseStart, segment->baseSize, m, baseType});
        tmpLocalBlockDataList.append(new CodeEditor::BlockData{segment->localStart, segment->localSize, m, localType});
        tmpRemoteBlockDataList.append(new CodeEditor::BlockData{segment->remoteStart, segment->remoteSize, m, remoteType});
        tmpResultBlockDataList.append(new CodeEditor::BlockData{segment->baseStart, segment->baseSize, m, resultType});
    }

    d->baseBlockDataList = tmpBaseBlockDataList;
    d->localBlockDataList = tmpLocalBlockDataList;
    d->remoteBlockDataList = tmpRemoteBlockDataList;
    d->resultBlockDataList = tmpResultBlockDataList;
    d->setEditorContents(d->lastMergeResult.segments);

    d->gotoNextDiffAction->setEnabled(d->conflictSegments.size());
    d->gotoPrevDiffAction->setEnabled(d->conflictSegments.size());

    d->keepMyFileAction->setEnabled(true);
    d->keepTheirFileAction->setEnabled(true);

    Q_EMIT isModifiedChanged(false);
    Q_EMIT conflictsChanged(d->conflictSegments.size());
}

QString MergeWidget::result() const
{
    return codeEditorResult->toPlainText();
}

bool MergeWidget::sameSize() const
{
    Q_D(const MergeWidget);
    return d->sameSize;
}

void MergeWidget::setSameSize(bool sameSize)
{
    Q_D(MergeWidget);
    d->sameSize = sameSize;
    d->setEditorContents(d->lastMergeResult.segments);
}

QAction *MergeWidget::keepMineAction()
{
    Q_D(const MergeWidget);
    return d->keepMineAction;
}

QAction *MergeWidget::keepTheirAction()
{
    Q_D(const MergeWidget);
    return d->keepTheirAction;
}

QAction *MergeWidget::keepMineBeforeTheirAction()
{
    Q_D(const MergeWidget);
    return d->keepMineBeforeTheirAction;
}

QAction *MergeWidget::keepTheirBeforeMineAction()
{
    Q_D(const MergeWidget);
    return d->keepTheirBeforeMineAction;
}

QAction *MergeWidget::keepMyFileAction()
{
    Q_D(const MergeWidget);
    return d->keepMyFileAction;
}

QAction *MergeWidget::keepTheirFileAction()
{
    Q_D(const MergeWidget);
    return d->keepTheirFileAction;
}

QAction *MergeWidget::gotoPrevDiffAction()
{
    Q_D(const MergeWidget);
    return d->gotoPrevDiffAction;
}

QAction *MergeWidget::gotoNextDiffAction()
{
    Q_D(const MergeWidget);
    return d->gotoNextDiffAction;
}

void MergeWidget::slotKeepMineActionTriggered()
{
    Q_D(MergeWidget);
    auto index = d->conflictIndexes.value(*d->currentSegment, -1);

    if (index == -1)
        return;

    codeEditorResult->setFrameText(index, (d->lastMergeResult.part(*d->currentSegment, Diff::MergeSide::Local)));
    d->resolveConflict(*d->currentSegment);
}

void MergeWidget::slotKeepTheirActionTriggered()
{
    Q_D(MergeWidget);
    auto index = d->conflictIndexes.value(*d->currentSegment, -1);

    if (index == -1)
        return;

    codeEditorResult->setFrameText(index, (d->lastMergeResult.part(*d->currentSegment, Diff::MergeSide::Remote)));
    d->resolveConflict(*d->currentSegment);
}

void MergeWidget::slotKeepMineBeforeTheirActionTriggered()
{
    Q_D(MergeWidget);
    auto index = d->conflictIndexes.value(*d->currentSegment, -1);

    if (index == -1)
        return;
    auto code = d->lastMergeResult.part(*d->currentSegment, Diff::MergeSide::Local);
    code.append(d->lastMergeResult.part(*d->currentSegment, Diff::MergeSide::Remote));
    codeEditorResult->setFrameText(index, code);
    d->resolveConflict(*d->currentSegment);
}

void MergeWidget::slotKeepTheirBeforeMineActionTriggered()
{
    Q_D(MergeWidget);
    auto index = d->conflictIndexes.value(*d->currentSegment, -1);

    if (index == -1)
        return;
    auto code = d->lastMergeResult.part(*d->currentSegment, Diff::MergeSide::Remote);
    code.append(d->lastMergeResult.part(*d->currentSegment, Diff::MergeSide::Local));
    codeEditorResult->setFrameText(index, code);
    d->resolveConflict(*d->currentSegment);
}

void MergeWidget::slotKeepMyFileActionTriggered()
{
    Q_D(MergeWidget);

    codeEditorResult->setPlainText(d->localContent);

    d->conflictIndexes.clear();
    d->conflictSegments.clear();
    Q_EMIT isModifiedChanged(true);
    Q_EMIT conflictsChanged(0);
}

void MergeWidget::slotKeepTheirFileActionTriggered()
{
    Q_D(MergeWidget);

    codeEditorResult->setPlainText(d->remoteContent);

    d->conflictIndexes.clear();
    d->conflictSegments.clear();
    Q_EMIT isModifiedChanged(true);
    Q_EMIT conflictsChanged(0);
}

void MergeWidget::slotGotoPrevDiffActionTriggered()
{
    Q_D(MergeWidget);

    auto pred = [](Diff::MergeSegment2 *segment) {
        return segment->type == Diff::MergeSegment2::Type::ChangedOnBoth;
    };

    auto &list = d->lastMergeResult.segments;
    QList<Diff::MergeSegment2 *>::iterator startPoint;
    if (d->currentSegment == list.end())
        startPoint = list.begin();
    else
        startPoint = d->currentSegment;

    --startPoint;
    auto it = std::find_if(std::make_reverse_iterator(startPoint), list.rend(), pred);

    if (it == list.rend())
        it = std::find_if(list.rbegin(), std::make_reverse_iterator(startPoint), pred);

    if (it != std::make_reverse_iterator(startPoint)) {
        qDebug() << "find";
        d->setCurrentSegment(--it.base());
    }
}

void MergeWidget::slotGotoNextDiffActionTriggered()
{
    Q_D(MergeWidget);

    auto pred = [](Diff::MergeSegment2 *segment) {
        return segment->type == Diff::MergeSegment2::Type::ChangedOnBoth;
    };

    auto &list = d->lastMergeResult.segments;
    QList<Diff::MergeSegment2 *>::iterator startPoint;
    if (d->currentSegment == list.end())
        startPoint = list.begin();
    else
        startPoint = d->currentSegment;

    ++startPoint;
    auto it = std::find_if(startPoint, list.end(), pred);

    if (it == list.end())
        it = std::find_if(list.begin(), startPoint, pred);

    if (it != startPoint) {
        qDebug() << "find";
        d->setCurrentSegment(it);
    }
}

void MergeWidget::slotCodeEditorScroll()
{
    auto s = qobject_cast<QScrollBar *>(sender());

    if (s != codeEditorBase->verticalScrollBar())
        codeEditorBase->verticalScrollBar()->setValue(s->value());

    if (s != codeEditorLocal->verticalScrollBar())
        codeEditorLocal->verticalScrollBar()->setValue(s->value());

    if (s != codeEditorRemote->verticalScrollBar())
        codeEditorRemote->verticalScrollBar()->setValue(s->value());

    if (s != codeEditorResult->verticalScrollBar())
        codeEditorResult->verticalScrollBar()->setValue(s->value());
}

void MergeWidget::slotCodeBlockSelected()
{
    Q_D(MergeWidget);
    auto editor = qobject_cast<CodeEditor *>(sender());
    auto ln = editor->currentLineNumber();
    qDebug() << "current line:" << ln;
    Diff::MergeSide side;

    if (editor == codeEditorBase)
        side = Diff::MergeSide::Base;
    else if (editor == codeEditorLocal)
        side = Diff::MergeSide::Local;
    else if (editor == codeEditorRemote)
        side = Diff::MergeSide::Remote;
    else
        return;

    auto it = std::find_if(d->lastMergeResult.segments.begin(), d->lastMergeResult.segments.end(), [&ln, &side](Diff::MergeSegment2 *segment) {
        switch (side) {
        case Diff::MergeSide::Base:
            return segment->baseStart <= ln && segment->baseStart + segment->baseSize >= ln;
        case Diff::MergeSide::Local:
            return segment->localStart <= ln && segment->localStart + segment->localSize >= ln;
        case Diff::MergeSide::Remote:
            return segment->remoteStart <= ln && segment->remoteStart + segment->remoteSize >= ln;
        }
        return false;
    });

    d->setCurrentSegment(it);
}

bool MergeWidget::isModified() const
{
    Q_D(const MergeWidget);
    return d->isModified;
}

void MergeWidget::setIsModified(bool isModified)
{
    Q_D(MergeWidget);
    if (d->isModified == isModified)
        return;
    d->isModified = isModified;
    emit isModifiedChanged(isModified);
}

int MergeWidget::conflicts() const
{
    Q_D(const MergeWidget);
    return d->conflictSegments.size();
}
