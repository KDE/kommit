/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <git2/blame.h>

#include <QList>
#include <QSharedPointer>

#include <Kommit/BlameHunk>

#include "libkommit_export.h"

namespace Git
{

class Repository;
class BlamePrivate;
class LIBKOMMIT_EXPORT Blame
{
public:
    Blame();
    Blame(Repository *gitManager, const QStringList &content, git_blame *blame);
    Blame(const Blame &other) = default;
    Blame &operator=(const Blame &other);
    bool operator==(const Blame &other) const;
    bool operator!=(const Blame &other) const;
    [[nodiscard]] bool isNull() const;

    [[nodiscard]] git_blame *data() const;
    [[nodiscard]] const git_blame *constData() const;

    [[nodiscard]] const QStringList &content() const;
    [[nodiscard]] QList<BlameHunk> hunks() const;
    [[nodiscard]] QString codeString(const BlameHunk &data) const;
    [[nodiscard]] QStringList codeLines(const BlameHunk &data) const;

    [[nodiscard]] const BlameHunk &at(int index) const;
    [[nodiscard]] BlameHunk hunkByLineNumber(int lineNumber) const;

    [[nodiscard]] QList<BlameHunk>::const_iterator begin() const;
    [[nodiscard]] QList<BlameHunk>::const_iterator end() const;
    [[nodiscard]] qsizetype size() const;

private:
    QSharedPointer<BlamePrivate> d;
};

}
