/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QObject>

class OverlayTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase();

    void test1();
    void checkRootDir();
    void dirTest();
};
