/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "configtest.h"
#include "testcommon.h"

#include <Kommit/Config>
#include <QTest>
#include <repository.h>

QTEST_GUILESS_MAIN(ConfigTest)

ConfigTest::ConfigTest(QObject *parent)
    : QObject{parent}
{
}

ConfigTest::~ConfigTest()
{
    delete mManager;
}

void ConfigTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Repository;
    QVERIFY(!mManager->isValid());

    auto ok = mManager->init(path);
    QVERIFY(ok);
    QCOMPARE(path, mManager->path());

    TestCommon::initSignature(mManager);
}

void ConfigTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void ConfigTest::setUserInfo()
{
    auto config = mManager->config();

    QVERIFY(!config.isNull());

    config.set("user.name", "kommit");
    config.set("user.email", "kommit@kde.org");

    QCOMPARE(config.valueString("user.name"), "kommit");
    QCOMPARE(config.valueString("user.email"), "kommit@kde.org");

    auto val = QUuid::createUuid().toString(QUuid::Id128);
    config.set("kommit.test", val);
    QCOMPARE(config.valueString("kommit.test"), val);
}

void ConfigTest::setGlobalConfig()
{
    auto val = QUuid::createUuid().toString(QUuid::Id128);
    auto config = Git::Repository::globalConfig();

    QVERIFY(!config.isNull());

    config.set("kommit.test", val);
    QCOMPARE(config.valueString("kommit.test"), val);
}

void ConfigTest::allConfigs()
{
    auto config = Git::Repository::globalConfig();

    auto userName = config.valueString("user.name");
    auto userEmail = config.valueString("user.email");

    auto userNameFound{false};
    auto userEmailFound{false};

    config.forEach([&](const QString &name, const QString &value) {
        if (name == "user.name" && value == userName)
            userNameFound = true;
        if (name == "user.email" && value == userEmail)
            userEmailFound = true;
        qDebug() << name << value;
    });

    QVERIFY(userNameFound);
    QVERIFY(userEmailFound);
}

#include "moc_configtest.cpp"
