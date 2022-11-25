/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "diff.h"

#include "gitklient_appdebug.h"

#include <QDebug>
#include <QDir>

#include <set>

namespace Diff
{

namespace Impl
{

using Pair2 = QPair<int, int>;
struct Pair3 {
    int first;
    int second;
    int third;

    Pair3()
        : first{-1}
        , second{-1}
        , third{-1}
    {
    }
    Pair3(int firstNumber, int secondNumber, int thirdNumber)
        : first{firstNumber}
        , second{secondNumber}
        , third{thirdNumber}
    {
    }

    bool operator==(Diff::Impl::Pair3 other) const
    {
        return first == other.first && second == other.second && third == other.third;
    }
};

QDebug operator<<(QDebug d, Diff::Impl::Pair3 p)
{
    d.noquote() << "(" << p.first << "," << p.second << "," << p.third << ")";
    return d;
}

using Solution = QList<Pair2>;
using Solution3 = QList<Pair3>;

class SolutionIterator
{
    const Solution &_solution;
    int _firstIndex{0};
    int _secondIndex{0};
    Solution::ConstIterator i;

public:
    struct Result {
        int oldStart;
        int newStart;
        int oldEnd;
        int newEnd;
        bool success;
        SegmentType type;

        Result()
            : success(false)
        {
        }
        Result(int oldStart, int oldEnd, int newStart, int newEnd, bool success, SegmentType type)
            : oldStart(oldStart)
            , newStart(newStart)
            , oldEnd(oldEnd)
            , newEnd(newEnd)
            , success(success)
            , type(type)
        {
        }
        int oldSize() const
        {
            return oldEnd - oldStart;
        }
        int newSize() const
        {
            return newEnd - newStart;
        }
    };

    SolutionIterator(const Solution &solution)
        : _solution(solution)
        , i(_solution.begin())
    {
    }
    void begin()
    {
        _firstIndex = _secondIndex = -1;
        i = _solution.begin();
    }
    Result pick()
    {
        if (i == _solution.end())
            return {};

        if ((i->first == _firstIndex && i->second == _secondIndex)) {
            auto fi = _firstIndex;
            auto si = _secondIndex;
            do {
                _firstIndex++;
                _secondIndex++;
                ++i;
            } while (i != _solution.end() && i->first == _firstIndex && i->second == _secondIndex);

            return {fi, _firstIndex, si, _secondIndex, true, SegmentType::SameOnBoth};
        } else {
            Result r{_firstIndex, i->first, _secondIndex, i->second, true, SegmentType::DifferentOnBoth};
            ++i;
            if (r.newStart == r.newEnd)
                r.type = SegmentType::OnlyOnLeft;
            else if (r.oldStart == r.oldEnd)
                r.type = SegmentType::OnlyOnRight;
            else
                r.type = SegmentType::DifferentOnBoth;
            _firstIndex = i->first;
            _secondIndex = i->second;
            return r;
        }

        return {};
    }
};

int maxIn(int first, int second, int third)
{
    if (first == second && second == third)
        return 0;

    if (third > first && third > second)
        return 3;

    if (first > second && first > third)
        return 1;

    if (second > first && second > third)
        return 2;

    return 4;
}

int maxIn(int first, int second)
{
    if (first == second)
        return 0;

    if (first > second)
        return 1;

    if (second > first)
        return 2;

    return 4;
}

int maxIn(const QList<int> &list)
{
    if (list.empty())
        return -1;
    int max{list.first()};
    int maxIndex{0};
    int index{0};
    for (const auto &i : list) {
        if (i > max) {
            max = i;
            maxIndex = index;
        }
        index++;
    }

    return maxIndex;
}

template<typename T>
class Array2
{
    T *_data;
    int c1, c2;

public:
    Array2(int c1, int c2);
    ~Array2();

    Q_ALWAYS_INLINE T &operator()(int i1, int i2);

    void exportToCsv(const QString &file);
};

template<typename T>
Q_OUTOFLINE_TEMPLATE Array2<T>::Array2(int c1, int c2)
    : c1(c1)
    , c2(c2)
{
    _data = new T[c1 * c2];
}

template<typename T>
Q_OUTOFLINE_TEMPLATE Array2<T>::~Array2()
{
    delete[] _data;
}

template<typename T>
Q_OUTOFLINE_TEMPLATE T &Array2<T>::operator()(int i1, int i2)
{
    return _data[c1 * i2 + i1];
}

template<typename T>
Q_OUTOFLINE_TEMPLATE void Array2<T>::exportToCsv(const QString &file)
{
    QFile f(file);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    for (int i = 0; i <= c1; i++) {
        for (int j = 0; j <= c2; j++)
            f.write(QByteArray::number(operator()(i, j)) + ",");
        f.write("\n");
    }
    f.close();
}
template<typename T>
class Array3
{
    T *_data;
    int c1, c2, c3;

public:
    Array3(int c1, int c2, int c3);
    ~Array3();

    Q_ALWAYS_INLINE T &operator()(int i1, int i2, int i3);
};

template<typename T>
Q_OUTOFLINE_TEMPLATE Array3<T>::Array3(int c1, int c2, int c3)
    : c1(c1)
    , c2(c2)
    , c3(c3)
{
    _data = new T[c1 * c2 * c3];
}

template<typename T>
Q_OUTOFLINE_TEMPLATE Array3<T>::~Array3()
{
    delete[] _data;
}

template<typename T>
Q_OUTOFLINE_TEMPLATE T &Array3<T>::operator()(int i1, int i2, int i3)
{
    return _data[i3 + (c2 * c1 * i1) + (c3 * i2)];
}

Solution3 longestCommonSubsequence(const QStringList &source, const QStringList &target, const QStringList &target2)
{
    Array3<int> l(source.size() + 1, target.size() + 1, target2.size() + 1);

    for (int i = 0; i <= source.count(); i++)
        for (int j = 0; j <= target.count(); j++)
            for (int k = 0; k <= target2.count(); ++k)
                if (i == 0 || j == 0 || k == 0) {
                    l(i, j, k) = 0;
                } else if (source.at(i - 1) == target.at(j - 1) && source.at(i - 1) == target2.at(k - 1)) {
                    l(i, j, k) = l(i - 1, j - 1, k - 1) + 1;
                } else {
                    l(i, j, k) = std::max(std::max(l(i - 1, j, k), l(i, j - 1, k)), l(i, j, k - 1));
                }

    int i = source.count();
    int j = target.count();
    int k = target2.count();
    int index = l(source.count(), target.count(), target2.count());

    Solution3 r;
    while (i > 0 && j > 0 && k > 0) {
        if (source.at(i - 1) == target.at(j - 1) && source.at(i - 1) == target2.at(k - 1)) {
            //            longestCommonSubsequence[index - 1] = source.at(i - 1);
            r.prepend({i - 1, j - 1, k - 1});
            i--;
            j--;
            k--;
            index--;
            ////        } else if (l[i - 1][j][k] > l[i][j - 1][k] && l[i - 1][j][k] > l[i][j][k - 1]) {
            ////            i--;
            ////        } else if (l[i][j][k - 1] > l[i][j - 1][k] && l[i][j][k - 1] > l[i - 1][j][k]) {
            ////            k--;
        } else {
            int n = maxIn({l(i - 1, j, k),
                           l(i, j - 1, k),
                           l(i, j, k - 1),

                           l(i - 1, j - 1, k),
                           l(i, j - 1, k - 1),
                           l(i - 1, j, k - 1),

                           l(i - 1, j - 1, k - 1)});
            switch (n) {
            case 0:
                i--;
                break;
            case 1:
                j--;
                break;
            case 2:
                k--;
                break;

            case 3:
                i--;
                j--;
                break;
            case 4:
                j--;
                k--;
                break;
            case 5:
                i--;
                k--;
                break;

            case 6:
                r.prepend({i - 1, j - 1, k - 1});
                i--;
                j--;
                k--;
                break;
            default:
                i--;
                j--;
                k--;
                break;
            }
            //            j--;
        }
    }
    return r;
}

template<typename T>
inline bool isEqual(const T &i1, const T &i2)
{
    return i1 == i2;
}
template<>
inline bool isEqual<QString>(const QString &i1, const QString &i2)
{
    return i1.trimmed() == i2.trimmed();
}

Solution longestCommonSubsequence(const QStringList &source, const QStringList &target)
{
    Array2<int> l(source.size() + 1, target.size() + 1);

    for (int i = 0; i <= source.count(); i++) {
        for (int j = 0; j <= target.count(); j++) {
            if (i == 0 || j == 0) {
                l(i, j) = 0;
            } else if (isEqual(source.at(i - 1), target.at(j - 1))) {
                l(i, j) = l(i - 1, j - 1) + 1;
            } else {
                l(i, j) = qMax(l(i - 1, j), l(i, j - 1));
            }
        }
    }

    int i = source.count();
    int j = target.count();
    Solution r;

    while (i > 0 && j > 0) {
        if (isEqual(source.at(i - 1), target.at(j - 1))) {
            r.prepend(qMakePair(i - 1, j - 1));
            i--;
            j--;
        } else {
            int n = maxIn(l(i - 1, j), l(i, j - 1), l(i - 1, j - 1));
            switch (n) {
            case 1:
                i--;
                break;
            case 2:
                j--;
                break;
            default:
                i--;
                j--;
                break;
            }
        }
    }

    return r;
}

}

Text readLines(const QString &text)
{
    if (text.isEmpty())
        return {};

    LineEnding le{LineEnding::None};
    QString seprator;
    for (const auto &ch : text) {
        if (le == LineEnding::Cr) {
            if (ch == QLatin1Char('\n')) {
                le = LineEnding::CrLf;
                seprator = QStringLiteral("\r\n");
            }
            break;
        }
        if (ch == QLatin1Char('\r')) {
            le = LineEnding::Cr;
            seprator = QStringLiteral("\r");
            continue;
        }
        if (ch == QLatin1Char('\n')) {
            seprator = QStringLiteral("\n");
            le = LineEnding::Lf;
            break;
        }
    }

    if (le == LineEnding::None) {
        qWarning() << "Unable to detect line ending";
        return {};
    }

    int i{0};
    Text t;
    t.lineEnding = le;
    while (i != -1) {
        auto n = text.indexOf(seprator, i);
        if (n == -1)
            break;
        t.lines.append(text.mid(i, n - i));
        i = n + seprator.size();
    }

    return t;
}

struct Tuple {
    int base;
    int local;
    int remote;
};

QDebug &operator<<(QDebug &stream, Diff::Tuple t)
{
    stream << t.base << t.local << t.remote;
    return stream;
}

QDebug &operator<<(QDebug &stream, const Diff::Impl::Solution &sln)
{
    for (auto i = sln.begin(); i != sln.end(); ++i)
        stream << "(" << i->first << ", " << i->second << ")";
    return stream;
}

QStringList readFileLines(const QString &filePath)
{
    //    QStringList buffer;
    QFile f{filePath};
    if (!f.open(QIODevice::ReadOnly))
        return {};

    //    while (!f.atEnd()) {
    //        buffer << f.readLine();
    //    }
    auto buf = QString(f.readAll()).split('\n');
    f.close();
    return buf;
}

Tuple firstCommonItem(const QStringList &base, const QStringList &local, const QStringList &remote)
{
    Tuple tmp{-1, -1, -1};
    for (int i = 0; i < base.size(); ++i) {
        const auto localIndex = local.indexOf(base.at(i));
        const auto remoteIndex = remote.indexOf(base.at(i));

        if (localIndex != -1 && remoteIndex != -1) {
            if (tmp.base == -1) {
                tmp.base = i;
                tmp.local = localIndex;
                tmp.remote = remoteIndex;
                continue;
            }
            auto sum = i + localIndex + remoteIndex;
            auto tmpSum = tmp.base + tmp.local + tmp.remote;
            if (sum < tmpSum) {
                tmp.base = i;
                tmp.local = localIndex;
                tmp.remote = remoteIndex;
                continue;
            }
        }
    }
    return tmp;
}

QPair<int, int> unmatchesCount(const QStringList &base, const QStringList &other)
{
    for (int i = 0; i < base.size(); ++i) {
        for (int j = 0; j < other.size(); ++j) {
            if (base.at(i) == other.at(j))
                return qMakePair(i, j);
        }
    }
    return qMakePair(-1, -1);
}

int matchesCount(const QStringList &base, const QStringList &local)
{
    int ret = 0;
    while (base.size() > ret && local.size() > ret && base.at(ret) == local.at(ret))
        ret++;
    return ret;
}

int matchesCount(const QStringList &base, const QStringList &local, const QStringList &remote)
{
    int ret = 0;
    while (base.size() > ret && local.size() > ret && remote.size() > ret && base.at(ret) == local.at(ret) && base.at(ret) == remote.at(ret))
        ret++;
    return ret;
}

QStringList take(QStringList &list, int count)
{
    QStringList ret = list.mid(0, qMin(count, list.size()));
    remove(list, count);
    return ret;
}

int remove(QStringList &list, int count)
{
    const int size{qMin<int>(count, list.size())};
    for (int i = 0; i < size; ++i)
        list.removeFirst();
    return size;
}

QList<MergeSegment *> diff3(const QStringList &baseList, const QStringList &localList, const QStringList &remoteList)
{
    auto max = Impl::longestCommonSubsequence(baseList, localList, remoteList);

    auto base = baseList;
    auto local = localList;
    auto remote = remoteList;

    int baseOffset{0};
    int localOffset{0};
    int remoteOffset{0};
    QList<MergeSegment *> ret;

    Impl::Pair3 p;
    while (!max.empty()) {
        if (p == Impl::Pair3())
            p = max.takeFirst();

        if (p.first == baseOffset && p.second == localOffset && p.third == remoteOffset) {
            auto segment = new MergeSegment;
            segment->type = SegmentType::SameOnBoth;
            while (p.first == baseOffset && p.second == localOffset && p.third == remoteOffset) {
                baseOffset++;
                localOffset++;
                remoteOffset++;
                segment->base.append(base.takeFirst());
                segment->local.append(local.takeFirst());
                segment->remote.append(remote.takeFirst());

                if (!max.empty())
                    p = max.takeFirst();
            }
            ret.append(segment);
            //            if (!max.size())
            //                break;
        } else {
            //            if (!max.size())
            //                break;
            //            p = max.takeFirst();
        }

        QStringList _baseList, _localList, _remoteList;
        if (baseOffset - p.first)
            _baseList = take(base, p.first - baseOffset);
        if (localOffset - p.second)
            _localList = take(local, p.second - localOffset);
        if (remoteOffset - p.third)
            _remoteList = take(remote, p.third - remoteOffset);

        baseOffset += _baseList.size();
        localOffset += _localList.size();
        remoteOffset += _remoteList.size();

        auto segment = new MergeSegment{_baseList, _localList, _remoteList};

        ret.append(segment);
    }

    if (base.empty() && local.empty() && remote.empty()) {
        auto segment = new MergeSegment{base, local, remote};
        ret.append(segment);
    }
    return ret;
}

MergeSegment::MergeSegment() = default;

MergeSegment::MergeSegment(const QStringList &base, const QStringList &local, const QStringList &remote)
    : base{base}
    , local{local}
    , remote{remote}
{
    if (local == remote)
        type = SegmentType::SameOnBoth;
    else if (base.empty() && !local.empty())
        type = SegmentType::OnlyOnLeft;
    else if (base.empty() && !remote.empty())
        type = SegmentType::OnlyOnRight;
    else
        type = SegmentType::DifferentOnBoth;

    oldText = local;
    newText = remote;

    if (type == SegmentType::SameOnBoth)
        this->base = this->local;
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

    auto lcs = Impl::longestCommonSubsequence(oldText, newText);

    Impl::SolutionIterator si(lcs);
    QList<DiffSegment *> ret;
    int lastLeftIndex{-1};
    int lastRightIndex{-1};
    forever {
        auto p = si.pick();
        if (!p.success)
            break;

        auto segment = new DiffSegment;
        segment->oldText = oldText.mid(p.oldStart, p.oldEnd - p.oldStart);
        segment->newText = newText.mid(p.newStart, p.newEnd - p.newStart);
        segment->type = p.type;
        lastLeftIndex = p.oldEnd;
        lastRightIndex = p.newEnd;
        ret << segment;
    }

    if (lastLeftIndex != oldText.size()) {
        auto segment = new DiffSegment;
        segment->oldText = oldText.mid(lastLeftIndex, -1);
        segment->newText = newText.mid(lastRightIndex, -1);
        segment->type = SegmentType::SameOnBoth;
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
