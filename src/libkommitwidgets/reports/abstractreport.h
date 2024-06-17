/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"

#include <QAbstractTableModel>

namespace Git
{
class Manager;
};

class LIBKOMMITWIDGETS_EXPORT AbstractReport : public QObject
{
    Q_OBJECT

public:
    explicit AbstractReport(Git::Manager *git, QObject *parent = nullptr);
    ~AbstractReport() override;

    virtual void reload() = 0;
    Q_REQUIRED_RESULT virtual QString name() const = 0;

    Q_REQUIRED_RESULT virtual bool supportChart() const;

    Q_REQUIRED_RESULT virtual QStringList headerData() const = 0;
    Q_REQUIRED_RESULT virtual int rowCount() const;
    Q_REQUIRED_RESULT virtual int columnCount() const = 0;
    Q_REQUIRED_RESULT QVariantList at(int index) const;
    Q_REQUIRED_RESULT QVariant at(int row, int column) const;

    Q_REQUIRED_RESULT virtual QString axisXTitle() const;
    Q_REQUIRED_RESULT virtual QString axisYTitle() const;
    Q_REQUIRED_RESULT int minValue() const;
    Q_REQUIRED_RESULT int maxValue() const;
    Q_REQUIRED_RESULT int categoryColumn() const;
    Q_REQUIRED_RESULT int valueColumn() const;
    Q_REQUIRED_RESULT virtual int labelsAngle() const;

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
    Git::Manager *const mGit;

    void clear();
    void setColumnCount(int columnCount);
    void extendRange(int value);
    void addData(std::initializer_list<QVariant> data);
    void setCategoryColumn(int categoryColumn);
    void setValueColumn(int valueColumn);
};
