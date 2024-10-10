/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#pragma once

#include <QtWidgets/QWidget>
namespace Git
{
class Repository;
}
class WidgetBase : public QWidget
{
    Q_OBJECT

    Git::Repository *_git{nullptr};

public:
    explicit WidgetBase(QWidget *parent = nullptr);
    explicit WidgetBase(Git::Repository *git, QWidget *parent = nullptr);
    Git::Repository *git() const;
    void setGit(Git::Repository *newGit);

    int exec(QWidget *parent = nullptr);

private slots:
    void gitPathChanged();
};
