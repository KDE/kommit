/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "libgitklientgui_private_export.h"
#include <QComboBox>
#include <QDialog>
#include <QMetaEnum>

namespace Git
{
class Manager;
}

class LIBGITKLIENTGUI_TESTS_EXPORT AppDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AppDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    explicit AppDialog(Git::Manager *git, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

    bool event(QEvent *event) override;

protected:
    Git::Manager *mGit = nullptr;

    template<typename _Enum>
    void initComboBox(QComboBox *comboBox)
    {
        comboBox->clear();
        QMetaEnum e = QMetaEnum::fromType<_Enum>();
        for (auto i = 0; i < e.keyCount(); i++) {
            QString name = e.key(i);
            comboBox->addItem(name, e.value(i));
        }
    }

    template<typename _Enum>
    _Enum comboBoxCurrentValue(QComboBox *comboBox) const
    {
        return static_cast<_Enum>(comboBox->currentData().toInt());
    }
};
