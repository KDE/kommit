/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "segmentsmapper.h"

#include "widgets/codeeditor.h"
#include <QScrollBar>

SegmentsMapper::SegmentsMapper(QObject *parent)
    : QObject(parent)
{
}

void SegmentsMapper::addEditor(CodeEditor *editor)
{
    mEditors.append(editor);

    connect(editor, &CodeEditor::blockSelected, this, &SegmentsMapper::codeEditor_blockSelected);

    mScrollBars.insert(editor->verticalScrollBar(), editor);
    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, &SegmentsMapper::codeEditor_scroll);
}

const QList<Diff::Segment *> &SegmentsMapper::segments() const
{
    return mSegments;
}

void SegmentsMapper::setSegments(const QList<Diff::MergeSegment *> &newSegments)
{
    for (const auto &s : newSegments)
        mSegments.append(s);
}

int SegmentsMapper::map(int from, int to, int index) const
{
    int offset1{0};
    int offset2{0};
    int offset3{0};
    int &offsetFrom = from == 1 ? offset1 : (from == 2 ? offset2 : offset3);
    int &offsetTo = to == 1 ? offset1 : (to == 2 ? offset2 : offset3);

    for (auto &s : mSegments) {
        auto ms = dynamic_cast<Diff::MergeSegment *>(s);

        if (offsetFrom + s->get(from).size() > index) {
            if (s->type != Diff::SegmentType::DifferentOnBoth)
                return offsetFrom + (index - offsetTo);
            else
                return offsetFrom;
        }

        offset1 += ms->base.size();
        offset2 += ms->local.size();
        offset3 += ms->remote.size();
    }
    return -1;
}

void SegmentsMapper::codeEditor_blockSelected()
{
    auto s = qobject_cast<CodeEditor *>(sender());

    mCurrentSegment = s->currentSegment();
    s->highlightSegment(mCurrentSegment);

    for (auto &editor : mEditors) {
        editor->highlightSegment(mCurrentSegment);
        editor->gotoSegment(mCurrentSegment);
        /*if (s == editor)
            continue;
        auto n = map(myIndx, _editors.indexOf(editor), l);

        if (n != -1)
            editor->gotoLineNumber(n);

        editor->highlightSegment(s->currentSegment());*/
    }
}

void SegmentsMapper::codeEditor_scroll(int value)
{
    static QAtomicInt n = 0;
    if (n)
        return;
    n.ref();
    auto s = mScrollBars.value(sender());
    if (!s)
        return;
    for (auto &editor : mEditors) {
        if (s == editor)
            continue;
        editor->verticalScrollBar()->setValue((int)(((float)value / (float)s->verticalScrollBar()->maximum()) * (float)s->verticalScrollBar()->maximum()));
    }
    n.deref();
}

Diff::Segment *SegmentsMapper::currentSegment() const
{
    return mCurrentSegment;
}

void SegmentsMapper::refresh()
{
    if (!mCurrentSegment)
        return;
    for (auto &editor : mEditors) {
        editor->highlightSegment(mCurrentSegment);
        editor->gotoSegment(mCurrentSegment);
    }
}

void SegmentsMapper::setCurrentSegment(Diff::Segment *newCurrentSegment)
{
    mCurrentSegment = newCurrentSegment;
    refresh();
}

bool SegmentsMapper::isMergeable() const
{
    for (auto &s : mSegments) {
        auto ms = dynamic_cast<Diff::MergeSegment *>(s);
        if (ms->mergeType == Diff::MergeType::None)
            return false;
    }
    return true;
}

int SegmentsMapper::conflicts() const
{
    int r{0};
    for (auto &s : mSegments) {
        auto ms = dynamic_cast<Diff::MergeSegment *>(s);
        if (ms->mergeType == Diff::None)
            r++;
    }
    return r;
}

void SegmentsMapper::findPrevious(Diff::SegmentType type)
{
    int index;
    if (mCurrentSegment)
        index = mSegments.indexOf(dynamic_cast<Diff::MergeSegment *>(mCurrentSegment)) - 1;
    else
        index = mSegments.size() - 1;

    for (auto i = index; i; i--)
        if (mSegments.at(i)->type == type) {
            setCurrentSegment(mSegments.at(i));
            return;
        }
}

void SegmentsMapper::findNext(Diff::SegmentType type)
{
    int index;
    if (mCurrentSegment)
        index = mSegments.indexOf(dynamic_cast<Diff::MergeSegment *>(mCurrentSegment)) + 1;
    else
        index = 0;

    for (auto i = index; i < mSegments.size(); i++)
        if (mSegments.at(i)->type == type) {
            setCurrentSegment(mSegments.at(i));
            return;
        }
}
