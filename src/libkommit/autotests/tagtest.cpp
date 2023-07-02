/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "tagtest.h"
#include "gittag.h"
#include <QTest>
QTEST_GUILESS_MAIN(TagTest)

TagTest::TagTest(QObject *parent)
    : QObject{parent}
{
}

void TagTest::shouldHaveDefaultValues()
{
    Git::Tag w;
    QVERIFY(w.message().isEmpty());
    QVERIFY(w.name().isEmpty());
    QVERIFY(w.taggerEmail().isEmpty());
}

#include "moc_tagtest.cpp"
