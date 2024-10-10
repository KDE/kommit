/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "gitloglist.h"
#include "libkommit_export.h"

#include "entities/oid.h"

#include <git2/blame.h>

#include <QScopedPointer>
#include <QSharedPointer>
#include <QString>

namespace Git
{

struct LIBKOMMIT_EXPORT BlameDataRow {
    BlameDataRow(Repository *git, const git_blame_hunk *hunk);

    const git_blame_hunk *const hunkPtr;

    const Oid commitId;

    const size_t startLine;
    const size_t linesCount;

    const QString originPath;

    const QSharedPointer<Commit> finalCommit;
    const QSharedPointer<Commit> originCommit;

    const QSharedPointer<Signature> finalSignature;
    const QSharedPointer<Signature> originSignature;

    const size_t finalStartLineNumber;
    const size_t originStartLineNumber;
};
bool operator==(const BlameDataRow &l, const BlameDataRow &r);
bool operator!=(const BlameDataRow &l, const BlameDataRow &r);

class BlameDataPrivate;
class LIBKOMMIT_EXPORT BlameData
{
public:
    BlameData(Repository *gitManager, const QStringList &content, git_blame *blame);
    ~BlameData();

    [[nodiscard]] const QStringList &content() const;
    [[nodiscard]] const QList<BlameDataRow *> hunks() const;
    [[nodiscard]] QString codeString(BlameDataRow *data) const;
    [[nodiscard]] QStringList codeLines(BlameDataRow *data) const;

    [[nodiscard]] const BlameDataRow *at(int index) const;
    [[nodiscard]] const BlameDataRow *hunkByLineNumber(int lineNumber) const;

    [[nodiscard]] QList<BlameDataRow *>::const_iterator begin() const;
    [[nodiscard]] QList<BlameDataRow *>::const_iterator end() const;
    [[nodiscard]] qsizetype size() const;

private:
    QScopedPointer<BlameDataPrivate> d_ptr;
    Q_DECLARE_PRIVATE(BlameData)
};

} // namespace Git
