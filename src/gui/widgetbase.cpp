/*
SPDX-FileCopyrightText: 2021 Hamed Masafi <hamed.masfi@gmail.com>

SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "widgetbase.h"

#include <repository.h>

#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>

WidgetBase::WidgetBase(QWidget *parent)
    : QWidget(parent)
{
    _git = Git::Repository::instance();
    connect(_git, &Git::Repository::pathChanged, this, &WidgetBase::git_pathChanged);
}

WidgetBase::WidgetBase(Git::Repository *git, QWidget *parent)
    : QWidget(parent)
    , _git(git)
{
    if (!_git)
        _git = Git::Repository::instance();
    connect(_git, &Git::Repository::pathChanged, this, &WidgetBase::git_pathChanged);
}

Git::Repository *WidgetBase::git() const
{
    return _git;
}

void WidgetBase::setGit(Git::Repository *newGit)
{
    _git = newGit;
}

int WidgetBase::exec(QWidget *parent)
{
    auto oldParent = this->parentWidget();
    QDialog d(parent);
    QVBoxLayout layout(&d);
    this->setParent(&d);
    layout.addWidget(this);
    //    layout.addWidget(new QPushButton(&d));
    int ret = d.exec();
    setParent(oldParent);
    return ret;
}

void WidgetBase::git_pathChanged()
{
    reload();
}

#include "moc_widgetbase.cpp"
