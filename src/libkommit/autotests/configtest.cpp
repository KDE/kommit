/*
SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "configtest.h"
#include "testcommon.h"

#include <QTest>
#include <gitmanager.h>

QTEST_GUILESS_MAIN(ConfigTest)

ConfigTest::ConfigTest(QObject *parent)
    : QObject{parent}
{
}

void ConfigTest::initTestCase()
{
    auto path = TestCommon::getTempPath();
    qDebug() << path;
    mManager = new Git::Manager;
    QVERIFY(!mManager->isValid());

    auto ok = mManager->init(path);
    QVERIFY(ok);
    QCOMPARE(path, mManager->path());
}

void ConfigTest::cleanupTestCase()
{
    TestCommon::cleanPath(mManager);
}

void ConfigTest::setUserInfo()
{
    mManager->setConfig("user.name", "kommit");
    mManager->setConfig("user.email", "kommit@kde.org");

    QCOMPARE(mManager->config("user.name"), "kommit");
    QCOMPARE(mManager->config("user.email"), "kommit@kde.org");

    auto val = QUuid::createUuid().toString(QUuid::Id128);
    mManager->setConfig("kommit.test", val);
    QCOMPARE(mManager->config("kommit.test"), val);
}

void ConfigTest::setGlobalConfig()
{
    auto val = QUuid::createUuid().toString(QUuid::Id128);
    mManager->setConfig("kommit.test", val, Git::Manager::ConfigGlobal);
    QCOMPARE(mManager->config("kommit.test", Git::Manager::ConfigGlobal), val);
}

void ConfigTest::allConfigs()
{
    auto userName = mManager->config("user.name", Git::Manager::ConfigGlobal);
    auto userEmail = mManager->config("user.email", Git::Manager::ConfigGlobal);

    auto userNameFound{false};
    auto userEmailFound{false};

    mManager->forEachConfig([&](QString name, QString val) {
        if (name == "user.name" && val == userName)
            userNameFound = true;
        if (name == "user.email" && val == userEmail)
            userEmailFound = true;
        qDebug() << name << val;
    });

    QVERIFY(userNameFound);
    QVERIFY(userEmailFound);
}

#include "moc_configtest.cpp"
