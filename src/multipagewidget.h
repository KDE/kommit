/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_multipagewidget.h"

#include <QIcon>

namespace Git {
class Manager;
}
class QActionGroup;
class WidgetBase;
class MultiPageWidget : public QWidget, private Ui::MultiPageWidget
{
    Q_OBJECT

    QActionGroup *const actionGroup;
    Git::Manager *_defaultGitManager{};
public:
    explicit MultiPageWidget(QWidget *parent = nullptr);
    void addPage(const QString &title, const QIcon &icon, WidgetBase *widget);
    void addPage(WidgetBase *widget, QAction *action);
    template<class T>
    void addPage() {
        addPage(new T(_defaultGitManager, this));
    }
    void setCurrentIndex(int index);
    QList<QAction *> actions() const;
    Git::Manager *defaultGitManager() const;
    void setDefaultGitManager(Git::Manager *newDefaultGitManager);

    int count() const;
private Q_SLOTS:
    void on_actionGroup_triggered(QAction *action);
};

