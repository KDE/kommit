/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diff2.h"

namespace Diff
{

Diff2TextResult diff(const QString &oldText, const QString &newText, const DiffOptions<QString> &opts)
{
    Text left, right;
    if (!oldText.isEmpty())
        left = readLines(oldText);
    if (!newText.isEmpty())
        right = readLines(newText);

    auto segments = Impl::diff(left.lines, right.lines, opts);
    return {left, right, segments};
}

class Diff2TextResultPrivate
{
public:
    ~Diff2TextResultPrivate();

    Text left;
    Text right;
    QList<DiffHunk *> hunks;
};

const Text &Diff2TextResult::left() const
{
    return d->left;
}

const Text &Diff2TextResult::right() const
{
    return d->right;
}

const QList<DiffHunk *> &Diff2TextResult::hunks() const
{
    return d->hunks;
}

Diff2TextResult::Diff2TextResult()
    : d{new Diff2TextResultPrivate}
{
}

Diff2TextResult::Diff2TextResult(Text left, Text right, QList<DiffHunk *> hunks)
    : d{new Diff2TextResultPrivate}
{
    d->left = std::move(left);
    d->right = std::move(right);
    d->hunks = std::move(hunks);
}

Diff2TextResult::Diff2TextResult(const Diff2TextResult &other)
    : d{other.d}
{
}

Diff2TextResult &Diff2TextResult::operator=(const Diff2TextResult &other)
{
    if (this != &other)
        d = other.d;

    return *this;
}

QList<DiffHunk *> diff2(const Text &oldText, const Text &newText, const DiffOptions<QString> &opts)
{
    return Impl::diff(oldText.lines, newText.lines, opts);
}

QList<DiffHunk *> diff2(const QStringList &oldText, const QStringList &newText, const DiffOptions<QString> &opts)
{
    return Impl::diff(oldText, newText, opts);
}

Diff2TextResultPrivate::~Diff2TextResultPrivate()
{
    qDeleteAll(hunks);
}

Diff2TextResult diff2(const QString &oldText, const QString &newText, const DiffOptions<QString> &opts)
{
    Text left = readLines(oldText);
    Text right = readLines(newText);
    auto segments = Impl::diff(left.lines, right.lines, opts);
    return Diff2TextResult{left, right, segments};
}
}
