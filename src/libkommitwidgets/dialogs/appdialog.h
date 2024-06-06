/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QComboBox>
#include <QDialog>
#include <QMetaEnum>

#include "libkommitwidgets_export.h"

namespace Git
{
class Manager;
}

class LIBKOMMITWIDGETS_EXPORT AppDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AppDialog(QWidget *parent = nullptr);
    explicit AppDialog(Git::Manager *git, QWidget *parent = nullptr);

    bool event(QEvent *event) override;

protected:
    Git::Manager *mGit = nullptr;

    template<typename _Enum>
    void initComboBox(QComboBox *comboBox)
    {
        comboBox->clear();
        QMetaEnum e = QMetaEnum::fromType<_Enum>();
        for (auto i = 0; i < e.keyCount(); i++) {
            const QString name = e.key(i);
            comboBox->addItem(name, e.value(i));
        }
    }

    template<typename _Enum>
    _Enum comboBoxCurrentValue(QComboBox *comboBox) const
    {
        return static_cast<_Enum>(comboBox->currentData().toInt());
    }

    template<typename _Enum>
    void setComboboxValue(QComboBox *comboBox, _Enum value)
    {
        for (int i = 0; i < comboBox->count(); i++)
            if (comboBox->itemData(i).toInt() == static_cast<int>(value)) {
                comboBox->setCurrentIndex(i);
                return;
            }
    }
};
