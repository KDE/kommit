/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#ifndef BRANCH_H
#define BRANCH_H

#include <QString>

class Branch
{
public:
    Branch();

private:
    enum class Status
    {

    };
    QString _localName;
    QString _remoteName;
};

#endif // BRANCH_H
