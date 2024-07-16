/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_multipagewidget.h"

#include <QIcon>

namespace Git
{
class Manager;
}
class QActionGroup;
class WidgetBase;
class MultiPageWidget : public QWidget, private Ui::MultiPageWidget
{
    Q_OBJECT

public:
    explicit MultiPageWidget(QWidget *parent = nullptr);
    void addPage(const QString &title, const QIcon &icon, WidgetBase *widget);
    void addPage(WidgetBase *widget, QAction *action);
    template<class T>
    void addPage()
    {
        addPage(new T(mDefaultGitManager, this));
    }
    void setCurrentIndex(int index);
    QList<QAction *> actions() const;
    Git::Manager *defaultGitManager() const;
    void setDefaultGitManager(Git::Manager *newDefaultGitManager);

    Q_REQUIRED_RESULT int count() const;

private:
    void slotPageSelected(QAction *action);
    QActionGroup *const mActionGroup;
    Git::Manager *mDefaultGitManager = nullptr;
};
