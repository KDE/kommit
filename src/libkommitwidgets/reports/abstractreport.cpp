/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "abstractreport.h"
#include "gitmanager.h"

AbstractReport::AbstractReport(Git::Manager *git, QObject *parent)
    : QObject{parent}
    , mGit{git}
{
}

AbstractReport::~AbstractReport()
{
}

bool AbstractReport::supportChart() const
{
    return false;
}

int AbstractReport::rowCount() const
{
    return mData.size();
}

QVariantList AbstractReport::at(int index) const
{
    if (index < 0 || index >= mData.size())
        return {};

    return mData.at(index);
}

QVariant AbstractReport::at(int row, int column) const
{
    if (row < 0 || row >= mData.size())
        return {};

    return mData.at(row).at(column);
}

QString AbstractReport::axisXTitle() const
{
    return {};
}

QString AbstractReport::axisYTitle() const
{
    return {};
}

int AbstractReport::minValue() const
{
    return mMinValue;
}

int AbstractReport::maxValue() const
{
    return mMaxValue;
}

int AbstractReport::categoryColumn() const
{
    return mCategoryColumn;
}

void AbstractReport::setCategoryColumn(int categoryColumn)
{
    mCategoryColumn = categoryColumn;
}

int AbstractReport::valueColumn() const
{
    return mValueColumn;
}

int AbstractReport::labelsAngle() const
{
    return 0;
}

void AbstractReport::setValueColumn(int valueColumn)
{
    mValueColumn = valueColumn;
}

void AbstractReport::clear()
{
    mData.clear();
    mMinValue = mMaxValue = 0;
}

void AbstractReport::setColumnCount(int columnCount)
{
    mColumnCount = columnCount;
}

void AbstractReport::extendRange(int value)
{
    mMinValue = qMin(mMinValue, value);
    mMaxValue = qMax(mMaxValue, value);
}

void AbstractReport::addData(std::initializer_list<QVariant> data)
{
    if (data.size() != columnCount())
        return;

    mData << data;
}

#include "moc_abstractreport.cpp"
