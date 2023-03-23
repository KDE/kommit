/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QFile>
#include <QtGlobal>

namespace Diff
{

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
    return _data[i3 + (c2 * c3 * i1) + (c3 * i2)];
}

}
