/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blamedata.h"
#include "caches/commitscache.h"
#include "gitmanager.h"

#include <git2/blame.h>

namespace Git
{

class BlameDataPrivate
{
    BlameData *q_ptr;
    Q_DECLARE_PUBLIC(BlameData)

public:
    explicit BlameDataPrivate(BlameData *parent);

    QList<BlameDataRow *> hunks;
    QStringList content;
    git_blame *blame{nullptr};
    Manager *git{nullptr};
};
BlameDataPrivate::BlameDataPrivate(BlameData *parent)
    : q_ptr{parent}
{
}

bool operator==(const BlameDataRow &l, const BlameDataRow &r)
{
    return l.finalCommit == r.finalCommit;
}
bool operator!=(const BlameDataRow &l, const BlameDataRow &r)
{
    return !operator==(l, r);
}

BlameData::BlameData(Manager *gitManager, const QStringList &content, git_blame *blame)
    : d_ptr{new BlameDataPrivate{this}}
{
    Q_D(BlameData);
    d->blame = blame;
    d->git = gitManager;
    d->content = content;

    auto count = git_blame_get_hunk_count(blame);
    d->hunks.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        auto hunk = git_blame_get_hunk_byindex(blame, i);
        d->hunks << new BlameDataRow{d->git, hunk};
    }
}

Git::BlameData::~BlameData()
{
    Q_D(BlameData);
    qDeleteAll(d->hunks);
    git_blame_free(d->blame);
    delete d;
}

const QStringList &BlameData::content() const
{
    Q_D(const BlameData);
    return d->content;
}

const QList<BlameDataRow *> BlameData::hunks() const
{
    Q_D(const BlameData);
    return d->hunks;
}

QString BlameData::codeString(BlameDataRow *data) const
{
    Q_D(const BlameData);
    return d->content.mid(data->startLine - 1, data->linesCount).join(QLatin1Char('\n'));
}

QStringList BlameData::codeLines(BlameDataRow *data) const
{
    Q_D(const BlameData);
    return d->content.mid(data->startLine - 1, data->linesCount);
}

const BlameDataRow *BlameData::at(int index) const
{
    Q_D(const BlameData);
    return d->hunks.at(index);
}

const BlameDataRow *BlameData::hunkByLineNumber(int lineNumber) const
{
    Q_D(const BlameData);
    auto hunk = git_blame_get_hunk_byline(d->blame, lineNumber);

    auto it = std::find_if(d->hunks.begin(), d->hunks.end(), [&hunk](BlameDataRow *row) {
        return row->hunkPtr == hunk;
    });

    if (it != d->hunks.end())
        return *it;

    return new BlameDataRow{d->git, hunk};
}

QList<BlameDataRow *>::const_iterator BlameData::begin() const
{
    Q_D(const BlameData);
    return d->hunks.begin();
}

QList<BlameDataRow *>::const_iterator BlameData::end() const
{
    Q_D(const BlameData);
    return d->hunks.end();
}

qsizetype BlameData::size() const
{
    Q_D(const BlameData);
    return d->hunks.size();
}

BlameDataRow::BlameDataRow(Manager *git, const git_blame_hunk *hunk)
    : hunkPtr{hunk}
    , commitId{&hunk->final_commit_id}
    , startLine{hunk->final_start_line_number}
    , linesCount{hunk->lines_in_hunk}
    , originPath{hunk->orig_path}
    , finalCommit{git->commits()->findByOid(&hunk->final_commit_id)}
    , originCommit{git->commits()->findByOid(&hunk->orig_commit_id)}
    , finalSignature{new Signature{hunk->orig_signature}}
    , originSignature{new Signature{hunk->final_signature}}
    , finalStartLineNumber{hunk->final_start_line_number}
    , originStartLineNumber{hunk->orig_start_line_number}
{
}
}
