/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommit_export.h"

#include <QAbstractTableModel>

namespace Git
{
class Manager;
};

class LIBKOMMIT_EXPORT AbstractReport : public QObject
{
    Q_OBJECT

public:
    AbstractReport(Git::Manager *git, QObject *parent = nullptr);
    virtual ~AbstractReport();

    virtual void reload() = 0;
    virtual QString name() const = 0;

    virtual bool supportChart() const;

    virtual QStringList headerData() const = 0;
    virtual int rowCount() const;
    virtual int columnCount() const = 0;
    QVariantList at(int index) const;
    QVariant at(int row, int column) const;
    Q_REQUIRED_RESULT virtual QString axisXTitle() const;
    Q_REQUIRED_RESULT virtual QString axisYTitle() const;

    Q_REQUIRED_RESULT int minValue() const;

    Q_REQUIRED_RESULT int maxValue() const;

    Q_REQUIRED_RESULT int categoryColumn() const;

    Q_REQUIRED_RESULT int valueColumn() const;

Q_SIGNALS:
    void reloaded();

private:
    QList<QVariantList> mData;
    int mColumnCount{};
    int mMinValue{};
    int mMaxValue{};
    int mCategoryColumn{0};
    int mValueColumn{1};

protected:
    void clear();
    void setColumnCount(int columnCount);
    void extendRange(int value);
    Git::Manager *mGit;
    void addData(std::initializer_list<QVariant> data);
    void setCategoryColumn(int categoryColumn);
    void setValueColumn(int valueColumn);
};
