/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "signaturetest.h"

#include "testcommon.h"

#include <Kommit/Repository>
#include <Kommit/Signature>
#include <QTest>

#include <QDateTime>
#include <git2.h>

QTEST_GUILESS_MAIN(SignatureTest)

SignatureTest::SignatureTest(QObject *parent)
    : QObject{parent}
    , mManager{new Git::Repository}
{
}

void SignatureTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;

    QVERIFY(TestCommon::extractSampleRepo(path));

    auto ok = mManager->open(path);
    QVERIFY(ok);
    TestCommon::initSignature(mManager);
}

void SignatureTest::emptysign()
{
    Git::Signature sign;
    QVERIFY(!sign.isNull());
}

void SignatureTest::defaultSign()
{
    git_signature *s;
    git_signature_default(&s, mManager->repoPtr());
    Git::Signature sign{s};

    QCOMPARE(sign.name(), "kommit test user");
    QCOMPARE(sign.email(), "kommit@kde.org");
}

void SignatureTest::nowSign()
{
    git_signature *s;
    git_signature_now(&s, "kommit test user", "kommit@kde.org");
    Git::Signature sign{s};

    QCOMPARE(sign.name(), "kommit test user");
    QCOMPARE(sign.email(), "kommit@kde.org");
    auto signTime = sign.time();
    auto now = QDateTime::currentDateTime();
    QCOMPARE(signTime.timeZone().offsetFromUtc(signTime), now.timeZone().offsetFromUtc(now));
    QCOMPARE(signTime.toString("HH:mm:ss"), now.toString("HH:mm:ss"));
}

#include "moc_signaturetest.cpp"
