/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "segmentsmapper.h"

#include "widgets/codeeditor.h"
#include <QScrollBar>

class SegmentsMapperPrivate
{
    SegmentsMapper *q_ptr;
    Q_DECLARE_PUBLIC(SegmentsMapper)

public:
    explicit SegmentsMapperPrivate(SegmentsMapper *parent);

    Diff::Segment *currentSegment{nullptr};
    QList<Diff::Segment *> segments;
    QList<CodeEditor *> editors;
    QMap<QObject *, CodeEditor *> scrollBars;
};

SegmentsMapper::SegmentsMapper(QObject *parent)
    : QObject{parent}
    , d_ptr{new SegmentsMapperPrivate{this}}
{
}

SegmentsMapper::~SegmentsMapper()
{
}

void SegmentsMapper::addEditor(CodeEditor *editor)
{
    Q_D(SegmentsMapper);
    d->editors.append(editor);

    connect(editor, &CodeEditor::blockSelected, this, &SegmentsMapper::codeEditor_blockSelected);

    d->scrollBars.insert(editor->verticalScrollBar(), editor);
    connect(editor->verticalScrollBar(), &QScrollBar::valueChanged, this, &SegmentsMapper::codeEditor_scroll);
}

const QList<Diff::Segment *> &SegmentsMapper::segments() const
{
    Q_D(const SegmentsMapper);
    return d->segments;
}

void SegmentsMapper::setSegments(const QList<Diff::MergeSegment *> &newSegments)
{
    Q_D(SegmentsMapper);
    for (const auto &s : newSegments)
        d->segments.append(s);
}

int SegmentsMapper::map(int from, int to, int index) const
{
    Q_D(const SegmentsMapper);
    int offset1{0};
    int offset2{0};
    int offset3{0};
    int &offsetFrom = from == 1 ? offset1 : (from == 2 ? offset2 : offset3);
    int &offsetTo = to == 1 ? offset1 : (to == 2 ? offset2 : offset3);

    for (auto &s : d->segments) {
        auto ms = static_cast<Diff::MergeSegment *>(s);

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
    Q_D(SegmentsMapper);
    auto s = qobject_cast<CodeEditor *>(sender());

    d->currentSegment = s->currentSegment();
    s->highlightSegment(d->currentSegment);

    for (auto &editor : d->editors) {
        editor->highlightSegment(d->currentSegment);
        editor->gotoSegment(d->currentSegment);
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
    Q_D(SegmentsMapper);
    static QAtomicInt n = 0;
    if (n)
        return;
    n.ref();
    auto s = d->scrollBars.value(sender());
    if (!s)
        return;
    for (auto &editor : d->editors) {
        if (s == editor)
            continue;
        editor->verticalScrollBar()->setValue((int)(((float)value / (float)s->verticalScrollBar()->maximum()) * (float)s->verticalScrollBar()->maximum()));
    }
    n.deref();
}

Diff::Segment *SegmentsMapper::currentSegment() const
{
    Q_D(const SegmentsMapper);
    return d->currentSegment;
}

void SegmentsMapper::refresh()
{
    Q_D(SegmentsMapper);
    if (!d->currentSegment)
        return;
    for (auto &editor : std::as_const(d->editors)) {
        editor->highlightSegment(d->currentSegment);
        editor->gotoSegment(d->currentSegment);
    }
}

void SegmentsMapper::setCurrentSegment(Diff::Segment *newCurrentSegment)
{
    Q_D(SegmentsMapper);
    d->currentSegment = newCurrentSegment;
    refresh();
}

bool SegmentsMapper::isMergeable() const
{
    Q_D(const SegmentsMapper);
    for (auto &s : d->segments) {
        auto ms = static_cast<Diff::MergeSegment *>(s);
        if (ms->mergeType == Diff::MergeType::None)
            return false;
    }
    return true;
}

int SegmentsMapper::conflicts() const
{
    Q_D(const SegmentsMapper);
    int r{0};
    for (auto &s : d->segments) {
        auto ms = static_cast<Diff::MergeSegment *>(s);
        if (ms->mergeType == Diff::None)
            r++;
    }
    return r;
}

void SegmentsMapper::findPrevious(Diff::SegmentType type)
{
    Q_D(SegmentsMapper);
    int index;
    if (d->currentSegment)
        index = d->segments.indexOf(dynamic_cast<Diff::MergeSegment *>(d->currentSegment)) - 1;
    else
        index = d->segments.size() - 1;

    if (index <= 0)
        return;
    for (auto i = index; i; i--)
        if (d->segments.at(i)->type == type) {
            setCurrentSegment(d->segments.at(i));
            return;
        }
}

void SegmentsMapper::findNext(Diff::SegmentType type)
{
    Q_D(SegmentsMapper);
    int index;
    if (d->currentSegment)
        index = d->segments.indexOf(dynamic_cast<Diff::MergeSegment *>(d->currentSegment)) + 1;
    else
        index = 0;

    for (auto i = index; i < d->segments.size(); i++)
        if (d->segments.at(i)->type == type) {
            setCurrentSegment(d->segments.at(i));
            return;
        }
}

#include "moc_segmentsmapper.cpp"

SegmentsMapperPrivate::SegmentsMapperPrivate(SegmentsMapper *parent)
    : q_ptr{parent}
{
}
