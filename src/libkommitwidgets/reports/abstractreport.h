/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libkommitwidgets_export.h"
#include <QObject>
#include <QVariant>
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
    [[nodiscard]] virtual QString name() const = 0;

    [[nodiscard]] virtual bool supportChart() const;

    [[nodiscard]] virtual QStringList headerData() const = 0;
    [[nodiscard]] virtual int rowCount() const;
    [[nodiscard]] virtual int columnCount() const = 0;
    [[nodiscard]] QVariantList at(int index) const;
    [[nodiscard]] QVariant at(int row, int column) const;

    [[nodiscard]] virtual QString axisXTitle() const;
    [[nodiscard]] virtual QString axisYTitle() const;
    [[nodiscard]] int minValue() const;
    [[nodiscard]] int maxValue() const;
    [[nodiscard]] int categoryColumn() const;
    [[nodiscard]] int valueColumn() const;
    [[nodiscard]] virtual int labelsAngle() const;

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
