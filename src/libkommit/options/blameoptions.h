/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QFlags>
#include <QScopedPointer>
#include <QSharedPointer>

#include <git2/blame.h>

namespace Git
{

class Commit;

class BlameOptionsPrivate;
class BlameOptions
{
    // Q_GADGET
public:
    BlameOptions();
    ~BlameOptions();

    enum class BlameFlag {
        Normal = GIT_BLAME_NORMAL,
        TrackCopiesSameFile = GIT_BLAME_TRACK_COPIES_SAME_FILE,
        TrackCopiesSameCommitMoves = GIT_BLAME_TRACK_COPIES_SAME_COMMIT_MOVES,
        TrackCopiesSameCommitCopies = GIT_BLAME_TRACK_COPIES_SAME_COMMIT_COPIES,
        TrackCopiesAnyCommitCopies = GIT_BLAME_TRACK_COPIES_ANY_COMMIT_COPIES,
        FirstParent = GIT_BLAME_FIRST_PARENT,
        UseMailmap = GIT_BLAME_USE_MAILMAP,
        IgnoreWhitespace = GIT_BLAME_IGNORE_WHITESPACE
    };

    Q_DECLARE_FLAGS(BlameFlags, BlameFlag)
    // Q_FLAG(BlameFlags)

    [[nodiscard]] QSharedPointer<Commit> firstCommit() const;
    void setFirstCommit(QSharedPointer<Commit> firstCommit);

    [[nodiscard]] QSharedPointer<Commit> lastCommit() const;
    void setLastCommit(QSharedPointer<Commit> lastCommit);

    [[nodiscard]] int minMatchCharacters() const;
    void setMinMatchCharacters(int minMatchCharacters);

    [[nodiscard]] int firstLineNumber() const;
    void setFirstLineNumber(int firstLineNumber);

    [[nodiscard]] int lastLineNumber() const;
    void setLastLineNumber(int lastLineNumber);

    void apply(git_blame_options *opts);
    [[nodiscard]] BlameFlags flags() const;
    void setFlags(const BlameFlags &flags);

private:
    QScopedPointer<BlameOptionsPrivate> d_ptr;
    Q_DECLARE_PRIVATE(BlameOptions)
};

}
