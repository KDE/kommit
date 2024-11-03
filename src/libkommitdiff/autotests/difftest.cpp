/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "difftest.h"

#include <QtTest>

#include <KommitDiff/Diff>

#include <lcs.h>
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
    // QStringList oldList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c"), QStringLiteral("e")};
    // QStringList newList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("d"), QStringLiteral("e")};
    // auto diffResult = Diff::diff2(oldList, newList);
    // QCOMPARE(diffResult.size(), 3);

    // QStringList f1{QStringLiteral("a"), QStringLiteral("b")};

    // auto r = diffResult.at(0);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    // QCOMPARE(r->oldText, f1);
    // QCOMPARE(r->newText, f1);

    // r = diffResult.at(1);
    // QCOMPARE(r->type, Diff::SegmentType::DifferentOnBoth);
    // QCOMPARE(r->oldText, oldList.mid(2, 1));
    // QCOMPARE(r->newText, newList.mid(2, 1));

    // r = diffResult.at(2);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    // QCOMPARE(r->oldText, oldList.mid(3, 1));
    // QCOMPARE(r->newText, newList.mid(3, 1));
}

void DiffTest::randomMissedNumber()
{
    // constexpr int total{100};

    // QStringList oldList;
    // QStringList newList;
    // auto n = 50; // QRandomGenerator::global()->bounded(2, 98);
    // for (auto i = 0; i < total; i++) {
    //     oldList << QString::number(i);
    //     newList << QString::number(i);
    // }
    // newList.removeAt(n);

    // auto diffResult = Diff::diff2(oldList, newList);

    // int oldCount{0};
    // int newCount{0};
    // for (const auto &r : diffResult) {
    //     oldCount += r->oldText.size();
    //     newCount += r->newText.size();
    // }

    // QCOMPARE(oldCount, 100);
    // QCOMPARE(newCount, 99);

    // QCOMPARE(diffResult.size(), 3);
    // auto r = diffResult.at(0);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    // QCOMPARE(r->oldText.size(), n);
    // QCOMPARE(r->newText.size(), n);

    // r = diffResult.at(1);
    // QCOMPARE(r->type, Diff::SegmentType::OnlyOnLeft);
    // QCOMPARE(r->oldText.size(), 1);
    // QCOMPARE(r->newText.size(), 0);

    // r = diffResult.at(2);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);

    // QCOMPARE(r->oldText.first().toInt(), n + 1);
    // QCOMPARE(r->newText.first().toInt(), n + 1);

    // QCOMPARE(r->oldText.size(), total - n - 1);
    // QCOMPARE(r->newText.size(), total - n - 1);
}

void DiffTest::removeFromLast()
{
    // QStringList oldList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
    // QStringList newList{QStringLiteral("a"), QStringLiteral("b")};
    // auto diffResult = Diff::diff2(oldList, newList);

    // QStringList ab{QStringLiteral("a"), QStringLiteral("b")};
    // QStringList c{QStringLiteral("c")};
    // QCOMPARE(diffResult.size(), 2);

    // auto r = diffResult.at(0);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    // QCOMPARE(r->oldText, ab);
    // QCOMPARE(r->newText, ab);

    // r = diffResult.at(1);
    // QCOMPARE(r->type, Diff::SegmentType::OnlyOnLeft);
    // QCOMPARE(r->oldText, c);
    // QCOMPARE(r->newText, QStringList());
}

void DiffTest::lcs()
{
    // QStringList l1{"a", "b", "c", "d"};
    // QStringList l2{"  A  ", "   b", "C   ", "d"};
    // auto result = Diff::longestCommonSubsequence(l1, l2, Diff::LcsOptions<QString>{true, true});
    // QCOMPARE(result.size(), 1);

    // auto first = result.at(0);
    // QCOMPARE(first.leftStart, 0);
    // QCOMPARE(first.leftEnd, 3);
    // QCOMPARE(first.rightStart, 0);
    // QCOMPARE(first.rightEnd, 3);
}

void DiffTest::simpleMerge()
{
    QStringList base{"a", "b", "c", "d", "e", "f"};
    QStringList local{"a", "b", "c", "d", "e", "f"};
    QStringList remote{"a", "b", /*"c",*/ "d", "e", "f"};

    auto result = Diff::diff3(base, local, remote);

    QCOMPARE(result.segments.size(), 3);

    QCOMPARE(result.part(result.segments[0], Diff::MergeSide::Base), (QStringList{"a", "b"}));
    QCOMPARE(result.part(result.segments[0], Diff::MergeSide::Local), (QStringList{"a", "b"}));
    QCOMPARE(result.part(result.segments[0], Diff::MergeSide::Remote), (QStringList{"a", "b"}));

    QCOMPARE(result.part(result.segments[1], Diff::MergeSide::Base), (QStringList{"c"}));
    QCOMPARE(result.part(result.segments[1], Diff::MergeSide::Local), (QStringList{"c"}));
    QCOMPARE(result.part(result.segments[1], Diff::MergeSide::Remote), (QStringList{}));

    QCOMPARE(result.part(result.segments[2], Diff::MergeSide::Base), (QStringList{"d", "e", "f"}));
    QCOMPARE(result.part(result.segments[2], Diff::MergeSide::Local), (QStringList{"d", "e", "f"}));
    QCOMPARE(result.part(result.segments[2], Diff::MergeSide::Remote), (QStringList{"d", "e", "f"}));
}

void DiffTest::removeFromLocal()
{
    QStringList base{"a", "b", "c", "d", "e"};
    QStringList local{"a", "c", "e"};
    QStringList remote{"a", "b", "c", "d", "e"};

    auto result = Diff::diff3(base, local, remote);

    QCOMPARE(result.segments.size(), 5);
}

void DiffTest::merge()
{
    Diff::DiffOptions<QString> opts;
    opts.ignoreCase = true;
    opts.ignoreWhiteSpaces = true;

    QStringList base{"a", "b", "c", "d", "e", "f", "g", "h", "i"};
    QStringList local{"z", "a", "b", "c2", "d2", "e", "f", "g1", "h", "i"};
    QStringList remote{"b", "c", "c1", "c2", "c3", "f", "g", "h", "i2"};

    auto result = Diff::diff3(base, local, remote, opts);

    QCOMPARE(result.segments.size(), 7);

    // auto basePart = result.part(result.segments[0], Diff::MergeSide::Base);

    // QCOMPARE(basePart, (QStringList{"a"}));
    // QCOMPARE(result.part(result.segments[0], Diff::MergeSide::Local), (QStringList{"z", "a"}));

    auto segment0 = result.segments[0];

    QCOMPARE(segment0->baseStart, 0);
    QCOMPARE(segment0->baseSize, 1);
    QCOMPARE(segment0->localStart, 0);
    QCOMPARE(segment0->localSize, 2);
    QCOMPARE(segment0->remoteStart, 0);
    QCOMPARE(segment0->remoteSize, 0);

    QCOMPARE(result.part(result.segments[1], Diff::MergeSide::Base), (QStringList{"b"}));
    QCOMPARE(result.part(result.segments[1], Diff::MergeSide::Local), (QStringList{"b"}));
    QCOMPARE(result.part(result.segments[1], Diff::MergeSide::Remote), (QStringList{"b"}));
}

void DiffTest::allPlacesRemove()
{
    // QStringList baseList{QStringLiteral("a"),
    //                      QStringLiteral("b"),
    //                      QStringLiteral("c"),
    //                      QStringLiteral("d"),
    //                      QStringLiteral("e"),
    //                      QStringLiteral("f"),
    //                      QStringLiteral("g"),
    //                      QStringLiteral("h"),
    //                      QStringLiteral("i"),
    //                      QStringLiteral("j")};
    // auto total = baseList.size();

    // for (auto i = 0; i < total; i++) {
    //     auto inTheMiddle{i && i < total - 1};

    //     for (auto t = 0; t < 2; t++) {
    //         auto oldList = baseList;
    //         auto newList = baseList;

    //         if (t)
    //             newList.removeAt(i);
    //         else
    //             oldList.removeAt(i);

    //         auto diffResult = Diff::diff2(oldList, newList);

    //         int oldCount{0};
    //         int newCount{0};
    //         for (const auto &r : diffResult) {
    //             oldCount += r->oldText.size();
    //             newCount += r->newText.size();
    //         }

    //         QCOMPARE(oldCount, total - (t ? 0 : 1));
    //         QCOMPARE(newCount, total - (t ? 1 : 0));
    //         QCOMPARE(diffResult.size(), inTheMiddle ? 3 : 2);
    //     }
    // }
}

void DiffTest::removedTest()
{
    // QStringList oldList{QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")};
    // QStringList newList{QStringLiteral("a"), QStringLiteral("c")};
    // auto diffResult = Diff::diff2(oldList, newList);

    // QStringList a{QStringLiteral("a")};
    // QStringList b{QStringLiteral("b")};
    // QStringList c{QStringLiteral("c")};
    // QCOMPARE(diffResult.size(), 3);

    // auto r = diffResult.at(0);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    // QCOMPARE(r->oldText, a);
    // QCOMPARE(r->newText, a);

    // r = diffResult.at(1);
    // QCOMPARE(r->type, Diff::SegmentType::OnlyOnLeft);
    // QCOMPARE(r->oldText, b);
    // QCOMPARE(r->newText, QStringList());

    // r = diffResult.at(2);
    // QCOMPARE(r->type, Diff::SegmentType::SameOnBoth);
    // QCOMPARE(r->oldText, c);
    // QCOMPARE(r->newText, c);
}

QTEST_MAIN(DiffTest)

#include "moc_difftest.cpp"
