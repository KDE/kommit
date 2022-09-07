/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef BLAMEDATA_H
#define BLAMEDATA_H

#include <QString>
#include "gitloglist.h"

namespace Git {

struct BlameDataRow {
    QString commitHash;
    QString code;

    Log *log;
};

class BlameData : public QList<BlameDataRow>
{
public:
    BlameData();

    void initCommits(const LogList &logs);
};

} // namespace Git

#endif // BLAMEDATA_H
