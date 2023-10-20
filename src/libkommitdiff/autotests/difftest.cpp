/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "difftest.h"

#include <QtTest>

#include <diff.h>
#include <solution.h>

void DiffTest::solutionTest()
{
    /*

    */
    Diff::Solution solution;
    solution << qMakePair(1, 1);
    solution << qMakePair(3, 4);
    solution << qMakePair(5, 6);
    Diff::SolutionIterator it{solution, 5, 6};
    it.begin();

    auto r = it.pick();
    QCOMPARE(r.success, true);
    QCOMPARE(r.oldStart, 0);
    QCOMPARE(r.newStart, 0);
}

void DiffTest::basicList()
{
    QStringList oldList{"a", "b", "c", "e"};
    QStringList newList{"a", "b", "d", "e"};
    auto diffResult = Diff::diff(oldList, newList);
    QCOMPARE(diffResult.size(), 3);

    QStringList f1{"a", "b"};

    auto r = diffResult.at(0);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    QCOMPARE(r->oldText, f1);
    QCOMPARE(r->newText, f1);

    r = diffResult.at(1);
    QCOMPARE(r->type, Diff::SegmentType::DifferentOnBoth);
    QCOMPARE(r->oldText, oldList.mid(2, 1));
    QCOMPARE(r->newText, newList.mid(2, 1));

    r = diffResult.at(2);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    QCOMPARE(r->oldText, oldList.mid(3, 1));
    QCOMPARE(r->newText, newList.mid(3, 1));
}

void DiffTest::randomMissedNumber()
{
    constexpr int total{100};

    QStringList oldList;
    QStringList newList;
    auto n = 50; // QRandomGenerator::global()->bounded(2, 98);
    for (auto i = 0; i < total; i++) {
        oldList << QString::number(i);
        newList << QString::number(i);
    }
    newList.removeAt(n);

    auto diffResult = Diff::diff(oldList, newList);

    int oldCount{0};
    int newCount{0};
    for (const auto &r : diffResult) {
        oldCount += r->oldText.size();
        newCount += r->newText.size();
    }

    QCOMPARE(oldCount, 100);
    QCOMPARE(newCount, 99);

    QCOMPARE(diffResult.size(), 3);
    auto r = diffResult.at(0);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    QCOMPARE(r->oldText.size(), n);
    QCOMPARE(r->newText.size(), n);

    r = diffResult.at(1);
    QCOMPARE(r->type, Diff::SegmentType::OnlyOnLeft);
    QCOMPARE(r->oldText.size(), 1);
    QCOMPARE(r->newText.size(), 0);

    r = diffResult.at(2);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);

    QCOMPARE(r->oldText.first().toInt(), n + 1);
    QCOMPARE(r->newText.first().toInt(), n + 1);

    QCOMPARE(r->oldText.size(), total - n - 1);
    QCOMPARE(r->newText.size(), total - n - 1);
}

void DiffTest::removeFromLast()
{
    QStringList oldList{"a", "b", "c"};
    QStringList newList{"a", "b"};
    auto diffResult = Diff::diff(oldList, newList);

    QStringList ab{"a", "b"};
    QStringList c{"c"};
    QCOMPARE(diffResult.size(), 2);

    auto r = diffResult.at(0);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    QCOMPARE(r->oldText, ab);
    QCOMPARE(r->newText, ab);

    r = diffResult.at(1);
    QCOMPARE(r->type, Diff::SegmentType::OnlyOnLeft);
    QCOMPARE(r->oldText, c);
    QCOMPARE(r->newText, QStringList());
}

void DiffTest::allPlacesRemove()
{
    QStringList baseList{"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    auto total = baseList.size();

    for (auto i = 0; i < total; i++) {
        auto inTheMiddle{i && i < total - 1};

        for (auto t = 0; t < 2; t++) {
            auto oldList = baseList;
            auto newList = baseList;

            if (t)
                newList.removeAt(i);
            else
                oldList.removeAt(i);

            auto diffResult = Diff::diff(oldList, newList);

            int oldCount{0};
            int newCount{0};
            for (const auto &r : diffResult) {
                oldCount += r->oldText.size();
                newCount += r->newText.size();
            }

            QCOMPARE(oldCount, total - (t ? 0 : 1));
            QCOMPARE(newCount, total - (t ? 1 : 0));
            QCOMPARE(diffResult.size(), inTheMiddle ? 3 : 2);
        }
    }
}

void DiffTest::removedTest()
{
    QStringList oldList{"a", "b", "c"};
    QStringList newList{"a", "c"};
    auto diffResult = Diff::diff(oldList, newList);

    QStringList a{"a"};
    QStringList b{"b"};
    QStringList c{"c"};
    QCOMPARE(diffResult.size(), 3);

    auto r = diffResult.at(0);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    QCOMPARE(r->oldText, a);
    QCOMPARE(r->newText, a);

    r = diffResult.at(1);
    QCOMPARE(r->type, Diff::SegmentType::OnlyOnLeft);
    QCOMPARE(r->oldText, b);
    QCOMPARE(r->newText, QStringList());

    r = diffResult.at(2);
    QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    QCOMPARE(r->oldText, c);
    QCOMPARE(r->newText, c);
}

QTEST_MAIN(DiffTest)

#include "moc_difftest.cpp"
