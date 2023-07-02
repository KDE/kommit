/*
    SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "filesstatuseslisttest.h"
#include "widgets/filesstatuseslist.h"

#include <QTest>

QTEST_MAIN(FilesStatusesListTest)
FilesStatusesListTest::FilesStatusesListTest(QObject *parent)
    : QObject{parent}
{
}

void FilesStatusesListTest::shouldHaveDefaultValues()
{
    FilesStatusesList w;
    QCOMPARE(w.columnCount(), 2);
}

#include "moc_filesstatuseslisttest.cpp"
