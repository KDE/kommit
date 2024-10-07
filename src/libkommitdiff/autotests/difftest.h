/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/
#pragma once

#include <QObject>

class DiffTest : public QObject
{
    Q_OBJECT
    void solutionTest();
    void basicList();
    void removedTest();
    void randomMissedNumber();
    void allPlacesRemove();
    void removeFromLast();
    void lcs();
    void merge();
    void simpleMerge();

private Q_SLOTS:
    void removeFromLocal();
};
