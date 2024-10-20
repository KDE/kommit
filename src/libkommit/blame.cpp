/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "blame.h"

#include "blamehunk.h"

namespace Git
{

class BlamePrivate
{
public:
    explicit BlamePrivate(git_blame *blame);
    ~BlamePrivate();

    QList<BlameHunk> hunks;
    QStringList content;
    git_blame *blame{nullptr};
    Repository *git{nullptr};
};
BlamePrivate::BlamePrivate(git_blame *blame)
    : blame{blame}
{
}

BlamePrivate::~BlamePrivate()
{
    git_blame_free(blame);
}

Blame::Blame()
    : d{new BlamePrivate{nullptr}}
{
}

Blame::Blame(Repository *gitManager, const QStringList &content, git_blame *blame)
    : d{new BlamePrivate{blame}}
{
    d->blame = blame;
    d->git = gitManager;
    d->content = content;

    auto count = git_blame_get_hunk_count(blame);
    d->hunks.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        auto hunk = git_blame_get_hunk_byindex(blame, i);
        d->hunks << BlameHunk{d->git, hunk};
    }
}

Blame::Blame(const Blame &other)
    : d{other.d}
{
}

Blame &Blame::operator=(const Blame &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

bool Blame::operator==(const Blame &other) const
{
    return d->blame == other.d->blame;
}

bool Blame::operator!=(const Blame &other) const
{
    return !(*this == other);
}

bool Blame::isNull() const
{
    return !d->blame;
}

git_blame *Blame::data() const
{
    return d->blame;
}

const git_blame *Blame::constData() const
{
    return d->blame;
}

const QStringList &Blame::content() const
{
    return d->content;
}

const QList<BlameHunk> Blame::hunks() const
{
    return d->hunks;
}

QString Blame::codeString(const BlameHunk &data) const
{
    return d->content.mid(data.startLine() - 1, data.linesCount()).join(QLatin1Char('\n'));
}

QStringList Blame::codeLines(const BlameHunk &data) const
{
    return d->content.mid(data.startLine() - 1, data.linesCount());
}

const BlameHunk &Blame::at(int index) const
{
    return d->hunks.at(index);
}

BlameHunk Blame::hunkByLineNumber(int lineNumber) const
{
    auto hunk = git_blame_get_hunk_byline(d->blame, lineNumber);

    auto it = std::find_if(d->hunks.begin(), d->hunks.end(), [&hunk](const BlameHunk &row) {
        return row.constData() == hunk;
    });

    if (it != d->hunks.end())
        return *it;

    return BlameHunk{d->git, hunk};
}

QList<BlameHunk>::const_iterator Blame::begin() const
{
    return d->hunks.begin();
}

QList<BlameHunk>::const_iterator Blame::end() const
{
    return d->hunks.end();
}

qsizetype Blame::size() const
{
    return d->hunks.size();
}

}
