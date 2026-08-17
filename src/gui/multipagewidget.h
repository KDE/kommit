/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include "ui_multipagewidget.h"

#include <QIcon>

namespace Git
{
class Repository;
}
class QActionGroup;
class WidgetBase;
class MultiPageWidget : public QWidget, private Ui::MultiPageWidget
{
    Q_OBJECT

public:
    explicit MultiPageWidget(QWidget *parent = nullptr);
    void addPage(const QString &title, const QIcon &icon, WidgetBase *widget);
    void addPage(WidgetBase *widget, QAction *action, const QIcon &icon);
    template<class T>
    void addPage()
    {
        addPage(new T(mDefaultGitManager, this));
    }
    void setCurrentIndex(int index);
    QList<QAction *> actions() const;
    Git::Repository *defaultGitManager() const;
    void setDefaultGitManager(Git::Repository *newDefaultGitManager);

    [[nodiscard]] int count() const;

protected:
    bool event(QEvent *event) override;

private:
    void updateStyleSheet();
    /// Gives the strip the width the longest of the names on it needs.
    void updateStripWidth();
    /// Works both out once the layout has settled, since a scroll bar appears during it.
    void scheduleStripUpdate();
    void slotPageSelected(QAction *action);
    QActionGroup *const mActionGroup;
    bool mStripUpdateScheduled{false};
    Git::Repository *mDefaultGitManager = nullptr;
    void updateTheme();
};
