/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QString>

class Branch
{
public:
    Branch();

private:
    enum class Status {

    };
    QString _localName;
    QString _remoteName;
};
