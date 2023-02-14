/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diff.h"

#include "array.h"
#include "lcs.h"
#include "pair.h"
#include "solution.h"
#include "text.h"

#include <QDebug>
#include <QDir>

#include <set>

namespace Diff
{

QStringList take(QStringList &list, int count)
{
    QStringList ret = list.mid(0, qMin(count, list.size()));
    remove(list, count);
    return ret;
}

int remove(QStringList &list, int count)
{
    const auto size{qMin<int>(count, list.size())};
    for (auto i = 0; i < size; ++i)
        list.removeFirst();
    return size;
}

QList<MergeSegment *> diff3(const QStringList &baseList, const QStringList &localList, const QStringList &remoteList)
{
    QList<MergeSegment *> ret;

    if (baseList.isEmpty()) {
        auto lcs = longestCommonSubsequence(localList, remoteList);
        SolutionIterator si(lcs, localList.size(), remoteList.size());

        si.begin();
        forever {
            auto p = si.pick();
            if (!p.success)
                break;

            auto segment = new MergeSegment;
            segment->local = localList.mid(p.oldStart, p.oldSize);
            segment->remote = remoteList.mid(p.newStart, p.newSize);
            segment->type = p.type;

            if (segment->type == SegmentType::SameOnBoth)
                segment->base = segment->local;

            ret << segment;
        }
    } else {
        auto lcs = longestCommonSubsequence(baseList, localList, remoteList);
        SolutionIterator3 si(lcs);
        forever {
            auto p = si.pick();
            if (!p.success)
                break;

            auto segment = new MergeSegment;
            segment->base = baseList.mid(p.base.begin, p.base.size);
            segment->local = localList.mid(p.local.begin, p.local.size);
            segment->remote = remoteList.mid(p.remote.begin, p.remote.size);
            segment->type = p.type;
            ret << segment;
        }
    }
    return ret;
}

QList<DiffSegment *> diff(const QStringList &oldText, const QStringList &newText)
{
    if (oldText == newText) {
        auto segment = new DiffSegment;
        segment->type = SegmentType::SameOnBoth;
        segment->oldText = oldText;
        segment->newText = newText;
        return {segment};
    } else if (oldText.isEmpty()) {
        auto segment = new DiffSegment;
        segment->type = SegmentType::OnlyOnLeft;
        segment->oldText = oldText;
        segment->newText = newText;
        return {segment};
    } else if (newText.isEmpty()) {
        auto segment = new DiffSegment;
        segment->type = SegmentType::OnlyOnRight;
        segment->oldText = oldText;
        segment->newText = newText;
        return {segment};
    }

    auto lcs = longestCommonSubsequence(oldText, newText);

    SolutionIterator si(lcs, oldText.size(), newText.size());
    QList<DiffSegment *> ret;

    si.begin();
    forever {
        auto p = si.pick();
        if (!p.success)
            break;

        if (!p.oldSize && !p.newSize)
            continue;

        auto segment = new DiffSegment;
        segment->oldText = oldText.mid(p.oldStart, p.oldSize);
        segment->newText = newText.mid(p.newStart, p.newSize);
        segment->type = p.type;
        ret << segment;
    }

    return ret;
}

QList<DiffSegment *> diff(const QString &oldText, const QString &newText)
{
    Text oldList, newList;
    if (!oldText.isEmpty())
        oldList = readLines(oldText);
    if (!newText.isEmpty())
        newList = readLines(newText);

    return diff(oldList.lines, newList.lines);
}

Diff2Result diff2(const QString &oldText, const QString &newText)
{
    Text oldList, newList;
    if (!oldText.isEmpty())
        oldList = readLines(oldText);
    if (!newText.isEmpty())
        newList = readLines(newText);

    Diff2Result result;
    result.oldTextLineEnding = oldList.lineEnding;
    result.newTextLineEnding = newList.lineEnding;
    result.segments = diff(oldList.lines, newList.lines);
    return result;
}

void browseDir(QStringList &filesList, const QString &dirPath, const QString &basePath)
{
    QDir d(basePath + QLatin1Char('/') + dirPath);
    const auto files = d.entryList(QDir::NoDotAndDotDot | QDir::Files);
    for (const auto &f : files) {
        if (dirPath.isEmpty())
            filesList.append(dirPath + f);
        else
            filesList.append(dirPath + QLatin1Char('/') + f);
    }

    const auto dirs = d.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
    for (const auto &d : dirs) {
        if (dirPath.isEmpty())
            browseDir(filesList, dirPath + d, basePath);
        else
            browseDir(filesList, dirPath + QLatin1Char('/') + d, basePath);
    }
}

bool isFilesSame(const QString &file1, const QString &file2)
{
    QFileInfo fi1{file1};
    QFileInfo fi2{file2};

    if (fi1.size() != fi2.size() /* || fi1.lastModified() != fi2.lastModified()*/)
        return false;

    QFile f1{file1};
    QFile f2{file2};

    if (!f1.open(QIODevice::ReadOnly) || !f2.open(QIODevice::ReadOnly))
        return false;

    while (!f1.atEnd() && !f2.atEnd()) {
        auto buffer1 = f1.read(100);
        auto buffer2 = f2.read(100);
        if (buffer1 != buffer2) {
            f1.close();
            f2.close();
            return false;
        }
    }

    f1.close();
    f2.close();

    return true;
}

QMap<QString, DiffType> diffDirs(const QString &dir1, const QString &dir2)
{
    auto d1 = dir1;
    auto d2 = dir2;
    QMap<QString, DiffType> map;

    if (!d1.endsWith(QLatin1Char('/')))
        d1.append(QLatin1Char('/'));

    if (!d2.endsWith(QLatin1Char('/')))
        d2.append(QLatin1Char('/'));

    QStringList files1, files2;
    browseDir(files1, {}, d1);
    browseDir(files2, {}, d2);

    for (const auto &file : std::as_const(files1)) {
        if (!files2.contains(file)) {
            map.insert(file, DiffType::Removed);
        } else {
            map.insert(file, isFilesSame(d1 + file, d2 + file) ? DiffType::Unchanged : DiffType::Modified);
            files2.removeOne(file);
        }
    }

    for (const auto &file : std::as_const(files2))
        map.insert(file, DiffType::Added);

    return map;
}

Diff3Result diff3(const QString &base, const QString &local, const QString &remote)
{
    Text baseList, localList, remoteList;
    if (!base.isEmpty())
        baseList = readLines(base);
    if (!local.isEmpty())
        localList = readLines(local);
    if (!remote.isEmpty())
        remoteList = readLines(remote);

    Diff3Result result;
    result.baseTextLineEnding = baseList.lineEnding;
    result.localTextLineEnding = localList.lineEnding;
    result.remoteTextLineEnding = remoteList.lineEnding;
    result.segments = diff3(baseList.lines, localList.lines, remoteList.lines);
    return result;
}
}
