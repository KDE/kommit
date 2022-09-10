/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QtWidgets/QWidget>
namespace Git {
class Manager;
}
class WidgetBase : public QWidget
{
    Q_OBJECT

    Git::Manager *_git{nullptr};

public:
    explicit WidgetBase(QWidget *parent = nullptr);
    explicit WidgetBase(Git::Manager *git, QWidget *parent = nullptr);
    Git::Manager *git() const;
    void setGit(Git::Manager *newGit);

    virtual void reload();

    int exec(QWidget *parent = nullptr);

private slots:
    void git_pathChanged();
};

