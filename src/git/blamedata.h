/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once
#include "libgitklient_export.h"
#include "gitloglist.h"
#include <QString>

namespace Git
{

struct BlameDataRow {
    QString commitHash;
    QString code;

    Log *log;
};

class LIBGITKLIENT_EXPORT BlameData : public QList<BlameDataRow>
{
public:
    BlameData();

    void initCommits(const LogList &logs);
};

} // namespace Git
