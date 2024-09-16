/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "gitloglist.h"
#include "libkommit_export.h"

#include <QSharedPointer>
#include <QString>

namespace Git
{

struct LIBKOMMIT_EXPORT BlameDataRow {
    BlameDataRow()
    {
    }
    BlameDataRow(QString commitHash,
                 QString code,
                 QString originPath,
                 QSharedPointer<Commit> finalCommit,
                 QSharedPointer<Commit> originCommit,
                 QSharedPointer<Signature> finalSignature,
                 QSharedPointer<Signature> originSignature,
                 size_t finalStartLineNumber,
                 size_t originStartLineNumber)
        : commitHash(std::move(commitHash))
        , code(std::move(code))
        , originPath(std::move(originPath))
        , finalCommit(std::move(finalCommit))
        , originCommit(std::move(originCommit))
        , finalSignature(std::move(finalSignature))
        , originSignature(std::move(originSignature))
        , finalStartLineNumber(finalStartLineNumber)
        , originStartLineNumber(originStartLineNumber)
    {
    }

    QString commitHash;
    QString code;

    QString originPath;
    QSharedPointer<Commit> finalCommit;
    QSharedPointer<Commit> originCommit;

    QSharedPointer<Signature> finalSignature;
    QSharedPointer<Signature> originSignature;

    size_t finalStartLineNumber;
    size_t originStartLineNumber;
};
bool operator==(const BlameDataRow &l, const BlameDataRow &r);
bool operator!=(const BlameDataRow &l, const BlameDataRow &r);

using BlameData = QList<BlameDataRow>;

} // namespace Git
